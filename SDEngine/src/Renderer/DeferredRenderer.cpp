#include "Renderer/DeferredRenderer.hpp"
#include "Core/Application.hpp"
#include "Renderer/Renderer3D.hpp"
#include "ECS/Component.hpp"

#include "Utility/Random.hpp"

#include "ImGui/ImGuiWidget.hpp"

#include "Loader/ShaderLoader.hpp"

#include "Asset/AssetStorage.hpp"
#include "Asset/ModelAsset.hpp"

#include "Core/Application.hpp"

namespace SD {

DataFormat GetTextureFormat(GeometryBufferType type)
{
    switch (type) {
        case GeometryBufferType::Position:
        case GeometryBufferType::Normal:
            return DataFormat::RGB16F;
        case GeometryBufferType::Albedo:
            return DataFormat::RGBA8;
        case GeometryBufferType::Ambient:
        case GeometryBufferType::Emissive:
            return DataFormat::RGB8;
        case GeometryBufferType::EntityId:
            return DataFormat::R32UI;
        default:
            SD_CORE_WARN("Unknown GBuffer!");
            return DataFormat::RGBA8;
    }
}

const int shadow_map_width = 4086;
const int shadow_map_height = 4086;
DeferredRenderer::DeferredRenderer(int width, int height, MultiSampleLevel msaa)
    : m_scene(nullptr),
      m_width(width),
      m_height(height),
      m_msaa(msaa),
      m_cascade_shadow(shadow_map_width, shadow_map_height),
      m_point_shadow(shadow_map_width, shadow_map_height),
      m_lighting_result(nullptr)
{
    InitShaders();
    InitSSAOKernel();
    InitSSAOBuffers();
    InitLightingBuffers();

    auto &settings = Application::GetApp().GetSettings();
    m_ssao_state = settings.GetBoolean("ssao", "state", true);
    m_ssao_radius = settings.GetFloat("ssao", "radius", 0.5);
    m_ssao_bias = settings.GetFloat("ssao", "bias", 0.25);
    m_ssao_power = settings.GetInteger("ssao", "power", 1);
}

DeferredRenderer::~DeferredRenderer()
{
    auto &settings = Application::GetApp().GetSettings();
    settings.SetBoolean("ssao", "state", m_ssao_state);
    settings.SetFloat("ssao", "radius", m_ssao_radius);
    settings.SetFloat("ssao", "bias", m_ssao_bias);
    settings.SetInteger("ssao", "power", m_ssao_power);
}

void DeferredRenderer::InitShaders()
{
    m_emssive_shader = ShaderLoader::LoadShader(
        "assets/shaders/quad.vert.glsl", "assets/shaders/emissive.frag.glsl");
    m_deferred_shader = ShaderLoader::LoadShader(
        "assets/shaders/quad.vert.glsl", "assets/shaders/deferred.frag.glsl");
    m_gbuffer_shader = ShaderLoader::LoadShader(
        "assets/shaders/mesh.vert.glsl", "assets/shaders/gbuffer.frag.glsl");

    m_ssao_shader = ShaderLoader::LoadShader("assets/shaders/ssao.comp.glsl");
    m_ssao_blur_shader =
        ShaderLoader::LoadShader("assets/shaders/ssao_blur.comp.glsl");

    m_cascade_shader =
        ShaderLoader::LoadShader("assets/shaders/shadow.vert.glsl", "",
                                 "assets/shaders/shadow.geo.glsl");
    m_cascade_debug_shader =
        ShaderLoader::LoadShader("assets/shaders/quad.vert.glsl",
                                 "assets/shaders/debug_depth.frag.glsl");
    m_point_shadow_shader =
        ShaderLoader::LoadShader("assets/shaders/shadow.vert.glsl",
                                 "assets/shaders/point_shadow.frag.glsl",
                                 "assets/shaders/point_shadow.geo.glsl");
}

void DeferredRenderer::InitSSAOBuffers()
{
    m_ssao_buffer =
        Texture::Create(m_width, m_height, 0, MultiSampleLevel::None,
                        TextureType::Normal, DataFormat::R16F);
    m_ssao_blur_buffer =
        Texture::Create(m_width, m_height, 0, MultiSampleLevel::None,
                        TextureType::Normal, DataFormat::R16F);
}

void DeferredRenderer::InitSSAOKernel()
{
    uint32_t kernel_size = m_ssao_shader->GetUint("u_kernel_size");
    m_ssao_kernel.resize(kernel_size);
    for (uint32_t i = 0; i < kernel_size; ++i) {
        Vector3f sample(Random::Rnd(-1.f, 1.f), Random::Rnd(-1.f, 1.f),
                        Random::Rnd(0.f, 1.0f));
        sample = glm::normalize(sample);
        sample *= Random::Rnd(0.f, 1.0f);
        float scale = static_cast<float>(i) / kernel_size;

        // scale samples s.t. they're more aligned to center of kernel
        scale = Math::Lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        m_ssao_kernel[i] = (sample);
    }

    std::array<Vector3f, 16> ssao_noise;
    for (uint32_t i = 0; i < 16; i++) {
        Vector3f noise(Random::Rnd(-1.f, 1.0f), Random::Rnd(-1.f, 1.0f),
                       0.0f);  // rotate around z-axis (in tangent space)
        ssao_noise[i] = glm::normalize(noise);
    }
    m_ssao_noise =
        Texture::Create(4, 4, 0, MultiSampleLevel::None, TextureType::Normal,
                        DataFormat::RGB16F, TextureWrap::Repeat);
    m_ssao_noise->SetPixels(0, 0, 0, 4, 4, 1, ssao_noise.data());

    m_ssao_shader->GetParam("u_samples[0]")
        ->SetAsVec3(&m_ssao_kernel[0][0], kernel_size);
}

void DeferredRenderer::InitLightingBuffers()
{
    for (int i = 0; i < 2; ++i) {
        m_lighting_target[i] = Framebuffer::Create();
        m_lighting_buffers[i] =
            Texture::Create(m_width, m_height, 0, m_msaa, TextureType::Normal,
                            DataFormat::RGB16F);
        m_lighting_target[i]->Attach(*m_lighting_buffers[i], 0, 0);
    }

    m_geometry_target_msaa = Framebuffer::Create();
    m_geometry_target = Framebuffer::Create();
    for (size_t i = 0; i < m_gbuffer_msaa.size(); ++i) {
        m_gbuffer_msaa[i] =
            Texture::Create(m_width, m_height, 0, m_msaa, TextureType::Normal,
                            GetTextureFormat(GeometryBufferType(i)));
        m_geometry_target_msaa->Attach(*m_gbuffer_msaa[i], i, 0);

        m_gbuffer[i] = Texture::Create(
            m_width, m_height, 0, MultiSampleLevel::None, TextureType::Normal,
            GetTextureFormat(GeometryBufferType(i)));
        m_geometry_target->Attach(*m_gbuffer[i], i, 0);
    }
    m_depth_buffer =
        Renderbuffer::Create(m_width, m_height, m_msaa, DataFormat::Depth24);
    m_geometry_target_msaa->Attach(*m_depth_buffer, 0);

    m_cascade_debug_target = Framebuffer::Create();
    m_cascade_debug_buffer =
        Texture::Create(m_width, m_height, 0, MultiSampleLevel::None,
                        TextureType::Normal, DataFormat::RGB16F);
    m_cascade_debug_target->Attach(*m_cascade_debug_buffer, 0, 0);
}

void DeferredRenderer::SetRenderSize(int32_t width, int32_t height)
{
    m_width = width;
    m_height = height;
    InitSSAOBuffers();
    InitLightingBuffers();
}

void DeferredRenderer::BlitGeometryBuffers()
{
    Renderer::BeginRenderPass({m_geometry_target.get(), m_width, m_height});
    for (size_t i = 0; i < m_gbuffer.size(); ++i) {
        Renderer::BlitFromBuffer(i, m_geometry_target_msaa.get(), i,
                                 BufferBitMask::ColorBufferBit);
    }
    Renderer::EndRenderPass();
}

void DeferredRenderer::ImGui()
{
    if (ImGui::TreeNodeEx("Geometry Buffers")) {
        for (size_t i = 0; i < m_gbuffer.size(); ++i) {
            ImGui::DrawTexture(*m_gbuffer[i], ImVec2(0, 1), ImVec2(1, 0));
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("SSAO")) {
        ImGui::Checkbox("On", &m_ssao_state);
        ImGui::TextUnformatted("SSAO Power");
        ImGui::SliderInt("##SSAO Power", &m_ssao_power, 1, 32);
        ImGui::TextUnformatted("SSAO Radius");
        ImGui::SliderFloat("##SSAO Radius", &m_ssao_radius, 0.1, 30);
        ImGui::TextUnformatted("SSAO Bias");
        ImGui::SliderFloat("##SSAO Bias", &m_ssao_bias, 0.01, 2);
        ImGui::DrawTexture(*m_ssao_blur_buffer, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Cascade Shadow")) {
        auto planes = m_cascade_shadow.GetCascadePlanes();
        int num_of_cascades = planes.size();
        if (ImGui::SliderInt("Num of Cascades", &num_of_cascades, 1, 4)) {
            m_cascade_shadow.SetNumOfCascades(num_of_cascades);
        }
        if (ImGui::InputFloat4("Cascades", planes.data())) {
            m_cascade_shadow.SetCascadePlanes(planes);
        }
        ImGui::InputInt("Layer", &m_debug_layer);
        ImGui::DrawTexture(*m_cascade_debug_buffer, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Point Shadow")) {
        float far_z = m_point_shadow.GetFarZ();
        if (ImGui::SliderFloat("Shadow Far Z", &far_z, 1.0f, 500.f)) {
            m_point_shadow.SetFarZ(far_z);
        }
        ImGui::TreePop();
    }
}

void DeferredRenderer::RenderScene(Scene &scene)
{
    m_scene = &scene;
    RenderGBuffer();
    BlitGeometryBuffers();
    if (m_ssao_state) {
        RenderSSAO();
    }
    RenderDeferred();
    RenderEmissive();

    Renderer::BlitFromBuffer(
        1, m_geometry_target_msaa.get(),
        static_cast<int>(GeometryBufferType::EntityId),
        BufferBitMask::ColorBufferBit | BufferBitMask::DepthBufferBit);
}

void DeferredRenderer::RenderShadowMap(const Transform &transform)
{
    auto modelView = m_scene->view<TransformComponent, ModelComponent>();
    RenderOperation op;
    op.cull_face = Face::Front;
    Texture *depth_map = m_cascade_shadow.GetShadowMap();
    Framebuffer *target = m_cascade_shadow.GetShadowTarget();
    Renderer::BeginRenderPass(RenderPassInfo{target, depth_map->GetWidth(),
                                             depth_map->GetHeight(), op});
    m_cascade_shadow.ComputeCascadeLightMatrix(transform,
                                               *Renderer::GetCamera());
    Renderer3D::SetShadowCaster(*m_cascade_shader, m_cascade_shadow);

    auto &storage = AssetStorage::Get();
    ShaderParam *model_param = m_cascade_shader->GetParam("u_model");
    modelView.each([&](const TransformComponent &transformComp,
                       const ModelComponent &modelComp) {
        if (storage.Exists<ModelAsset>(modelComp.model_id)) {
            auto model =
                storage.GetAsset<ModelAsset>(modelComp.model_id)->GetModel();
            for (const auto &[material, meshes] : model->GetMaterialMap()) {
                for (const auto &mesh : meshes) {
                    Matrix4f mat =
                        model->GetTransform(mesh.transform_id) *
                        transformComp.GetWorldTransform().GetMatrix();
                    model_param->SetAsMat4(&mat[0][0]);
                    Renderer3D::DrawMesh(*m_cascade_shader,
                                         *model->GetMesh(mesh.mesh_id));
                }
            }
        }
    });
    Renderer::EndRenderPass();

    // debug
    {
        Camera *cam = Renderer::GetCamera();
        Renderer::BeginRenderPass(RenderPassInfo{
            m_cascade_debug_target.get(), m_cascade_debug_buffer->GetWidth(),
            m_cascade_debug_buffer->GetHeight(), RenderOperation{}});
        m_cascade_debug_shader->GetParam("depthMap")->SetAsTexture(depth_map);
        m_cascade_debug_shader->GetParam("layer")->SetAsInt(m_debug_layer);
        m_cascade_debug_shader->GetParam("near_plane")
            ->SetAsFloat(cam->GetNearZ());
        m_cascade_debug_shader->GetParam("far_plane")
            ->SetAsFloat(cam->GetFarZ());
        Renderer::DrawNDCQuad(*m_cascade_debug_shader);

        Renderer::EndRenderPass();
    }
}

void DeferredRenderer::RenderPointShadowMap(const Transform &transform)
{
    Vector3f light_pos = transform.GetPosition();
    std::array<Matrix4f, 6> shadow_trans =
        m_point_shadow.GetProjectionMatrix(light_pos);

    auto modelView = m_scene->view<TransformComponent, ModelComponent>();
    RenderOperation op;
    op.cull_face = Face::Front;
    Texture *shadow_map = m_point_shadow.GetShadowMap();
    Framebuffer *shadow_target = m_point_shadow.GetShadowTarget();
    Renderer::BeginRenderPass(RenderPassInfo{
        shadow_target, shadow_map->GetWidth(), shadow_map->GetHeight(), op});

    auto &storage = AssetStorage::Get();
    ShaderParam *model_param = m_point_shadow_shader->GetParam("u_model");
    m_point_shadow_shader->GetParam("u_light_pos")
        ->SetAsVec3(&transform.GetPosition()[0]);
    m_point_shadow_shader->GetParam("u_far_z")->SetAsFloat(
        m_point_shadow.GetFarZ());
    m_point_shadow_shader->GetParam("u_shadow_matrix[0]")
        ->SetAsMat4(&shadow_trans[0][0][0], 6);
    modelView.each([&](const TransformComponent &transformComp,
                       const ModelComponent &modelComp) {
        if (storage.Exists<ModelAsset>(modelComp.model_id)) {
            auto model =
                storage.GetAsset<ModelAsset>(modelComp.model_id)->GetModel();
            for (const auto &[material, meshes] : model->GetMaterialMap()) {
                for (const auto &mesh : meshes) {
                    Matrix4f mat =
                        model->GetTransform(mesh.transform_id) *
                        transformComp.GetWorldTransform().GetMatrix();
                    model_param->SetAsMat4(&mat[0][0]);
                    Renderer3D::DrawMesh(*m_point_shadow_shader,
                                         *model->GetMesh(mesh.mesh_id));
                }
            }
        }
    });
    Renderer::EndRenderPass();
}

void DeferredRenderer::RenderSSAO()
{
    Texture *position =
        m_gbuffer[static_cast<int>(GeometryBufferType::Position)].get();
    Texture *normal =
        m_gbuffer[static_cast<int>(GeometryBufferType::Normal)].get();
    Renderer::BindCamera(*m_ssao_shader);
    m_ssao_shader->GetParam("u_radius")->SetAsFloat(m_ssao_radius);
    m_ssao_shader->GetParam("u_bias")->SetAsFloat(m_ssao_bias);
    m_ssao_shader->GetParam("u_power")->SetAsUint(m_ssao_power);
    m_ssao_shader->GetParam("u_position")->SetAsTexture(position);
    m_ssao_shader->GetParam("u_normal")->SetAsTexture(normal);
    m_ssao_shader->GetParam("u_noise")->SetAsTexture(m_ssao_noise.get());
    m_ssao_shader->GetParam("u_out_image")
        ->SetAsImage(m_ssao_buffer.get(), 0, false, 0, Access::WriteOnly);
    Renderer::ComputeImage(*m_ssao_shader, std::ceil(m_width / 25.f),
                              std::ceil(m_height / 25.f), 1);

    // blur
    m_ssao_blur_shader->GetParam("u_input")->SetAsTexture(m_ssao_buffer.get());
    m_ssao_blur_shader->GetParam("u_out_image")
        ->SetAsImage(m_ssao_blur_buffer.get(), 0, false, 0, Access::WriteOnly);
    Renderer::ComputeImage(*m_ssao_blur_shader, std::ceil(m_width / 25.f),
                              std::ceil(m_height / 25.f), 1);
}

void DeferredRenderer::RenderEmissive()
{
    auto dir_lights =
        m_scene->view<TransformComponent, DirectionalLightComponent>();
    auto point_lights =
        m_scene->view<TransformComponent, PointLightComponent>();
    RenderOperation op;
    op.blend = false;
    if (dir_lights.begin() != dir_lights.end() ||
        point_lights.begin() != point_lights.end()) {
        const int buffer = 0;
        Renderer::BeginRenderSubpass(RenderSubpassInfo{&buffer, 1, op});
        m_emssive_shader->GetParam("u_lighting")
            ->SetAsTexture(m_lighting_result);
        m_emssive_shader->GetParam("u_emissive")
            ->SetAsTexture(
                m_gbuffer_msaa[static_cast<int>(GeometryBufferType::Emissive)]
                    .get());
        Renderer::DrawNDCQuad(*m_emssive_shader);
        Renderer::EndRenderSubpass();
    }
}

void DeferredRenderer::RenderDeferred()
{
    m_deferred_shader->GetParam("u_position")
        ->SetAsTexture(
            m_gbuffer_msaa[static_cast<int>(GeometryBufferType::Position)]
                .get());
    m_deferred_shader->GetParam("u_normal")
        ->SetAsTexture(
            m_gbuffer_msaa[static_cast<int>(GeometryBufferType::Normal)].get());
    m_deferred_shader->GetParam("u_albedo")
        ->SetAsTexture(
            m_gbuffer_msaa[static_cast<int>(GeometryBufferType::Albedo)].get());
    m_deferred_shader->GetParam("u_ambient")
        ->SetAsTexture(
            m_gbuffer_msaa[static_cast<int>(GeometryBufferType::Ambient)]
                .get());
    m_deferred_shader->GetParam("u_background")
        ->SetAsTexture(
            Renderer::GetCurrentRenderPass().framebuffer->GetAttachment(0));
    m_deferred_shader->GetParam("u_ssao")->SetAsTexture(
        m_ssao_blur_buffer.get());
    m_deferred_shader->GetParam("u_ssao_state")->SetAsBool(m_ssao_state);

    ShaderParam *lighting = m_deferred_shader->GetParam("u_lighting");
    ShaderParam *ambient = m_deferred_shader->GetParam("u_light_color.ambient");
    ShaderParam *diffuse = m_deferred_shader->GetParam("u_light_color.diffuse");
    ShaderParam *specular =
        m_deferred_shader->GetParam("u_light_color.specular");
    ShaderParam *light_front = m_deferred_shader->GetParam("u_light_front");
    ShaderParam *is_directional =
        m_deferred_shader->GetParam("u_is_directional");
    ShaderParam *is_cast_shadow =
        m_deferred_shader->GetParam("u_is_cast_shadow");

    ShaderParam *cascade_map = m_deferred_shader->GetParam("u_cascade_map");
    ShaderParam *num_of_cascades =
        m_deferred_shader->GetParam("u_num_of_cascades");
    ShaderParam *cascade_planes =
        m_deferred_shader->GetParam("u_cascade_planes[0]");

    Renderer::BindCamera(*m_deferred_shader);

    RenderOperation op;
    op.blend = false;

    uint8_t input_id = 0;
    uint8_t output_id = 1;
    // clear the last lighting pass' result
    const float value[]{0, 0, 0, 0};
    m_lighting_target[input_id]->ClearAttachment(0, value);
    auto dir_lights =
        m_scene->view<TransformComponent, DirectionalLightComponent>();
    dir_lights.each([&](const TransformComponent &transformComp,
                        DirectionalLightComponent &lightComp) {
        DirectionalLight &light = lightComp.light;
        const Transform &transform = transformComp.GetWorldTransform();

        RenderPassInfo info{m_lighting_target[output_id].get(),
                            m_lighting_buffers[output_id]->GetWidth(),
                            m_lighting_buffers[output_id]->GetHeight(),
                            op,
                            BufferBitMask::ColorBufferBit,
                            {0, 0, 0, 0}};
        m_lighting_result = m_lighting_buffers[output_id].get();

        Renderer::BeginRenderPass(info);

        lighting->SetAsTexture(m_lighting_buffers[input_id].get());
        light_front->SetAsVec3(&transform.GetFront()[0]);
        ambient->SetAsVec3(&light.ambient[0]);
        diffuse->SetAsVec3(&light.diffuse[0]);
        specular->SetAsVec3(&light.specular[0]);

        is_directional->SetAsBool(true);
        is_cast_shadow->SetAsBool(lightComp.is_cast_shadow);
        if (lightComp.is_cast_shadow) {
            RenderShadowMap(transform);

            cascade_map->SetAsTexture(m_cascade_shadow.GetShadowMap());
            auto &planes = m_cascade_shadow.GetCascadePlanes();
            num_of_cascades->SetAsInt(planes.size());
            cascade_planes->SetAsVec(&planes[0], planes.size());
            Renderer3D::SetShadowCaster(*m_deferred_shader, m_cascade_shadow);
        }
        else {
            cascade_map->SetAsTexture(nullptr);
        }
        Renderer::DrawNDCQuad(*m_deferred_shader);
        std::swap(input_id, output_id);
        Renderer::EndRenderPass();
    });

    ShaderParam *light_position =
        m_deferred_shader->GetParam("u_point_light.pos");
    ShaderParam *constant =
        m_deferred_shader->GetParam("u_point_light.constant");
    ShaderParam *linear = m_deferred_shader->GetParam("u_point_light.linear");
    ShaderParam *quadratic =
        m_deferred_shader->GetParam("u_point_light.quadratic");
    ShaderParam *point_shadow_map =
        m_deferred_shader->GetParam("u_point_shadow_map");
    ShaderParam *point_shadow_far_z =
        m_deferred_shader->GetParam("u_point_shadow_far_z");
    auto point_lights =
        m_scene->view<TransformComponent, PointLightComponent>();
    point_lights.each([&](const TransformComponent &transformComp,
                          PointLightComponent &lightComp) {
        PointLight &light = lightComp.light;
        const Transform &transform = transformComp.GetWorldTransform();

        RenderPassInfo info{m_lighting_target[output_id].get(),
                            m_lighting_buffers[output_id]->GetWidth(),
                            m_lighting_buffers[output_id]->GetHeight(),
                            op,
                            BufferBitMask::ColorBufferBit,
                            {0, 0, 0, 0}};
        m_lighting_result = m_lighting_buffers[output_id].get();

        Renderer::BeginRenderPass(info);

        lighting->SetAsTexture(m_lighting_buffers[input_id].get());
        light_front->SetAsVec3(&transform.GetFront()[0]);
        light_position->SetAsVec3(&transform.GetPosition()[0]);
        ambient->SetAsVec3(&light.ambient[0]);
        diffuse->SetAsVec3(&light.diffuse[0]);
        specular->SetAsVec3(&light.specular[0]);
        constant->SetAsFloat(light.constant);
        linear->SetAsFloat(light.linear);
        quadratic->SetAsFloat(light.quadratic);

        is_directional->SetAsBool(false);
        is_cast_shadow->SetAsBool(lightComp.is_cast_shadow);
        if (lightComp.is_cast_shadow) {
            RenderPointShadowMap(transform);
            point_shadow_map->SetAsTexture(m_point_shadow.GetShadowMap());
            point_shadow_far_z->SetAsFloat(m_point_shadow.GetFarZ());
        }
        else {
            point_shadow_map->SetAsTexture(nullptr);
        }
        Renderer::DrawNDCQuad(*m_deferred_shader);
        std::swap(input_id, output_id);
        Renderer::EndRenderPass();
    });
}

void DeferredRenderer::RenderGBuffer()
{
    auto modelView = m_scene->view<TransformComponent, ModelComponent>();

    RenderPassInfo info;
    info.framebuffer = m_geometry_target_msaa.get();
    info.viewport_width = m_width;
    info.viewport_height = m_height;
    info.clear_mask =
        BufferBitMask::ColorBufferBit | BufferBitMask::DepthBufferBit;
    info.clear_value = {0, 0, 0, 0};
    info.op.blend = false;
    Renderer::BeginRenderPass(info);
    Renderer::BindCamera(*m_gbuffer_shader);
    uint32_t id = static_cast<uint32_t>(entt::null);
    m_geometry_target_msaa->ClearAttachment(
        static_cast<int>(GeometryBufferType::EntityId), &id);

    auto &storage = AssetStorage::Get();
    ShaderParam *entity_id = m_gbuffer_shader->GetParam("u_entity_id");
    ShaderParam *model_param = m_gbuffer_shader->GetParam("u_model");
    modelView.each([&](const entt::entity &entity,
                       const TransformComponent &transformComp,
                       const ModelComponent &modelComp) {
        entity_id->SetAsUint(static_cast<uint32_t>(entity));
        if (storage.Exists<ModelAsset>(modelComp.model_id)) {
            auto model =
                storage.GetAsset<ModelAsset>(modelComp.model_id)->GetModel();
            for (const auto &[material, meshes] : model->GetMaterialMap()) {
                Renderer3D::SetMaterial(*m_gbuffer_shader, *material);
                for (const auto &mesh : meshes) {
                    Matrix4f mat =
                        model->GetTransform(mesh.transform_id) *
                        transformComp.GetWorldTransform().GetMatrix();
                    model_param->SetAsMat4(&mat[0][0]);
                    Renderer3D::DrawMesh(*m_gbuffer_shader,
                                         *model->GetMesh(mesh.mesh_id));
                }
            }
        }
    });
    Renderer::EndRenderPass();
}

}  // namespace SD
