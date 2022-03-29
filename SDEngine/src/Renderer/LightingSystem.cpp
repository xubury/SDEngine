#include "Renderer/LightingSystem.hpp"
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

LightingSystem::LightingSystem(int width, int height, MultiSampleLevel msaa)
    : RenderSystem("LightingSystem"),
      m_width(width),
      m_height(height),
      m_msaa(msaa)
{
}

void LightingSystem::OnInit()
{
    RenderSystem::OnInit();
    InitShaders();
    InitSSAOKernel();
    InitSSAO();
    InitLighting();
}

void LightingSystem::OnPush()
{
    auto &dispatcher = GetEventDispatcher();
    m_size_handler = dispatcher.Register(this, &LightingSystem::OnSizeEvent);

    auto &settings = Application::GetApp().GetSettings();
    m_ssao_state = settings.GetBoolean("ssao", "state", true);
    m_ssao_radius = settings.GetFloat("ssao", "radius", 0.5);
    m_ssao_bias = settings.GetFloat("ssao", "bias", 0.25);
    m_ssao_power = settings.GetInteger("ssao", "power", 1);
}

void LightingSystem::OnPop()
{
    auto &dispatcher = GetEventDispatcher();
    dispatcher.RemoveHandler(m_size_handler);

    auto &settings = Application::GetApp().GetSettings();
    settings.SetBoolean("ssao", "state", m_ssao_state);
    settings.SetFloat("ssao", "radius", m_ssao_radius);
    settings.SetFloat("ssao", "bias", m_ssao_bias);
    settings.SetInteger("ssao", "power", m_ssao_power);
}

void LightingSystem::InitShaders()
{
    m_emssive_shader = ShaderLoader::LoadShader(
        "assets/shaders/quad.vert.glsl", "assets/shaders/emissive.frag.glsl");
    m_deferred_shader = ShaderLoader::LoadShader(
        "assets/shaders/quad.vert.glsl", "assets/shaders/deferred.frag.glsl");
    m_gbuffer_shader = ShaderLoader::LoadShader(
        "assets/shaders/mesh.vert.glsl", "assets/shaders/gbuffer.frag.glsl");
    m_ssao_shader = ShaderLoader::LoadShader("assets/shaders/quad.vert.glsl",
                                             "assets/shaders/ssao.frag.glsl");
    m_ssao_blur_shader = ShaderLoader::LoadShader(
        "assets/shaders/quad.vert.glsl", "assets/shaders/ssao_blur.frag.glsl");

    m_cascade_shader =
        ShaderLoader::LoadShader("assets/shaders/shadow.vert.glsl", "",
                                 "assets/shaders/shadow.geo.glsl");
    m_cascade_debug_shader =
        ShaderLoader::LoadShader("assets/shaders/quad.vert.glsl",
                                 "assets/shaders/debug_depth.frag.glsl");
}

void LightingSystem::InitSSAO()
{
    // ssao target
    AttachmentDescription attach_desc{AttachmentType::Normal, DataFormat::R16F,
                                      MultiSampleLevel::None};

    m_ssao_buffer = Framebuffer::Create({m_width, m_height, 0, {attach_desc}});
    m_ssao_blur_buffer =
        Framebuffer::Create({m_width, m_height, 0, {attach_desc}});
}

void LightingSystem::InitSSAOKernel()
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

void LightingSystem::InitLighting()
{
    for (int i = 0; i < 2; ++i) {
        m_light_buffer[i] = Framebuffer::Create(
            {m_width,
             m_height,
             0,
             {{AttachmentType::Normal, DataFormat::RGB16F, m_msaa}}});
    }

    FramebufferCreateInfo info;
    info.width = m_width;
    info.height = m_height;
    for (int i = 0; i < int(GeometryBufferType::GBufferCount); ++i) {
        info.attachments.push_back(AttachmentDescription{
            AttachmentType::Normal, GetTextureFormat(GeometryBufferType(i)),
            m_msaa});
    }
    info.attachments.push_back(AttachmentDescription{
        AttachmentType::ReadOnly, DataFormat::Depth24, m_msaa});
    m_gbuffer = Framebuffer::Create(info);

    m_cascade_debug_fb = Framebuffer::Create(
        {m_width,
         m_height,
         0,
         {AttachmentDescription{AttachmentType::Normal, DataFormat::RGB8,
                                MultiSampleLevel::None}}});
}

void LightingSystem::OnSizeEvent(const RenderSizeEvent &event)
{
    m_width = event.width;
    m_height = event.height;
    for (auto &buffer : m_light_buffer) {
        buffer->Resize(event.width, event.height);
    }
    m_gbuffer->Resize(event.width, event.height);
    m_ssao_buffer->Resize(event.width, event.height);
    m_ssao_blur_buffer->Resize(event.width, event.height);
}

void LightingSystem::OnImGui()
{
    ImGui::Begin("Lighting System");
    {
        ImGui::Checkbox("SSAO", &m_ssao_state);
        ImGui::TextUnformatted("SSAO Power");
        ImGui::SliderInt("##SSAO Power", &m_ssao_power, 1, 32);
        ImGui::TextUnformatted("SSAO Radius");
        ImGui::SliderFloat("##SSAO Radius", &m_ssao_radius, 0.1, 30);
        ImGui::TextUnformatted("SSAO Bias");
        ImGui::SliderFloat("##SSAO Bias", &m_ssao_bias, 0.01, 2);
    }
    ImGui::End();
    ImGui::Begin("Depth Map");
    ImGui::InputInt("Layer", &m_debug_layer);
    ImGui::DrawTexture(*m_cascade_debug_fb->GetTexture());
    ImGui::End();
}

void LightingSystem::OnRender()
{
    RenderGBuffer();
    if (m_ssao_state) {
        RenderSSAO();
    }
    RenderDeferred();
    RenderEmissive();

    Renderer::BlitFromBuffer(
        1, m_gbuffer.get(), static_cast<int>(GeometryBufferType::EntityId),
        BufferBitMask::ColorBufferBit | BufferBitMask::DepthBufferBit);
}

void LightingSystem::RenderShadowMap(CascadeShadow &shadow,
                                     const Transform &transform)
{
    auto modelView = GetScene().view<TransformComponent, ModelComponent>();
    RenderOperation op;
    op.cull_face = Face::Front;
    Framebuffer *depth_map = shadow.GetCascadeMap();
    Renderer::BeginRenderPass(RenderPassInfo{depth_map, depth_map->GetWidth(),
                                             depth_map->GetHeight(), op});
    shadow.ComputeCascadeLightMatrix(transform, GetCamera());
    Renderer3D::SetShadowCaster(*m_cascade_shader, shadow);

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
        Renderer::BeginRenderPass(RenderPassInfo{
            m_cascade_debug_fb.get(), m_cascade_debug_fb->GetWidth(),
            m_cascade_debug_fb->GetHeight(), RenderOperation{}});
        m_cascade_debug_shader->GetParam("depthMap")
            ->SetAsTexture(shadow.GetCascadeMap()->GetTexture());
        m_cascade_debug_shader->GetParam("layer")->SetAsInt(m_debug_layer);
        m_cascade_debug_shader->GetParam("near_plane")
            ->SetAsFloat(GetCamera().GetNearZ());
        m_cascade_debug_shader->GetParam("far_plane")
            ->SetAsFloat(GetCamera().GetFarZ());
        Renderer::DrawNDCQuad(*m_cascade_debug_shader);

        Renderer::EndRenderPass();
    }
}

void LightingSystem::RenderSSAO()
{
    RenderPassInfo info;
    info.framebuffer = m_ssao_buffer.get();
    info.viewport_width = m_ssao_buffer->GetWidth();
    info.viewport_height = m_ssao_buffer->GetHeight();
    info.op.blend = false;
    info.clear_value = {1.0f, 1.0f, 1.0f, 1.f};
    Texture *position =
        m_gbuffer->GetTexture(static_cast<int>(GeometryBufferType::Position));
    Texture *normal =
        m_gbuffer->GetTexture(static_cast<int>(GeometryBufferType::Normal));
    Renderer::BeginRenderPass(info);
    Renderer::SetCamera(*m_ssao_shader, GetCamera());
    m_ssao_shader->GetParam("u_radius")->SetAsFloat(m_ssao_radius);
    m_ssao_shader->GetParam("u_bias")->SetAsFloat(m_ssao_bias);
    m_ssao_shader->GetParam("u_power")->SetAsUint(m_ssao_power);
    m_ssao_shader->GetParam("u_position")->SetAsTexture(position);
    m_ssao_shader->GetParam("u_normal")->SetAsTexture(normal);
    m_ssao_shader->GetParam("u_noise")->SetAsTexture(m_ssao_noise.get());
    Renderer::DrawNDCQuad(*m_ssao_shader);

    Renderer::EndRenderPass();

    // blur
    info.framebuffer = m_ssao_blur_buffer.get();
    info.viewport_width = m_ssao_blur_buffer->GetWidth();
    info.viewport_height = m_ssao_blur_buffer->GetHeight();
    info.clear_value = {1.0f, 1.0f, 1.0f, 1.f};
    info.op.blend = false;
    Renderer::BeginRenderPass(info);
    m_ssao_blur_shader->GetParam("u_ssao")->SetAsTexture(
        m_ssao_buffer->GetTexture());
    Renderer::DrawNDCQuad(*m_ssao_blur_shader);
    Renderer::EndRenderPass();
}

void LightingSystem::RenderEmissive()
{
    auto lightView = GetScene().view<TransformComponent, LightComponent>();
    RenderOperation op;
    op.blend = false;
    if (lightView.begin() != lightView.end()) {
        const int buffer = 0;
        Renderer::BeginRenderSubpass(RenderSubpassInfo{&buffer, 1, op});
        m_emssive_shader->GetParam("u_lighting")
            ->SetAsTexture(GetLightingBuffer()->GetTexture());
        m_emssive_shader->GetParam("u_emissive")
            ->SetAsTexture(m_gbuffer->GetTexture(
                static_cast<int>(GeometryBufferType::Emissive)));
        Renderer::DrawNDCQuad(*m_emssive_shader);
        Renderer::EndRenderSubpass();
    }
}

void LightingSystem::RenderDeferred()
{
    auto lightView = GetScene().view<TransformComponent, LightComponent>();

    m_deferred_shader->GetParam("u_position")
        ->SetAsTexture(m_gbuffer->GetTexture(
            static_cast<int>(GeometryBufferType::Position)));
    m_deferred_shader->GetParam("u_normal")
        ->SetAsTexture(m_gbuffer->GetTexture(
            static_cast<int>(GeometryBufferType::Normal)));
    m_deferred_shader->GetParam("u_albedo")
        ->SetAsTexture(m_gbuffer->GetTexture(
            static_cast<int>(GeometryBufferType::Albedo)));
    m_deferred_shader->GetParam("u_ambient")
        ->SetAsTexture(m_gbuffer->GetTexture(
            static_cast<int>(GeometryBufferType::Ambient)));
    m_deferred_shader->GetParam("u_background")
        ->SetAsTexture(
            Renderer::GetCurrentRenderPass().framebuffer->GetTexture());
    m_deferred_shader->GetParam("u_ssao")->SetAsTexture(
        m_ssao_blur_buffer->GetTexture());
    m_deferred_shader->GetParam("u_ssao_state")->SetAsBool(m_ssao_state);

    ShaderParam *lighting = m_deferred_shader->GetParam("u_lighting");
    ShaderParam *ambient = m_deferred_shader->GetParam("u_light.ambient");
    ShaderParam *diffuse = m_deferred_shader->GetParam("u_light.diffuse");
    ShaderParam *specular = m_deferred_shader->GetParam("u_light.specular");
    ShaderParam *position = m_deferred_shader->GetParam("u_light.position");
    ShaderParam *direction = m_deferred_shader->GetParam("u_light.direction");
    ShaderParam *cutoff = m_deferred_shader->GetParam("u_light.cutoff");
    ShaderParam *outer_cutoff =
        m_deferred_shader->GetParam("u_light.outer_cutoff");
    ShaderParam *constant = m_deferred_shader->GetParam("u_light.constant");
    ShaderParam *linear = m_deferred_shader->GetParam("u_light.linear");
    ShaderParam *quadratic = m_deferred_shader->GetParam("u_light.quadratic");
    ShaderParam *is_directional =
        m_deferred_shader->GetParam("u_light.is_directional");
    ShaderParam *is_cast_shadow =
        m_deferred_shader->GetParam("u_light.is_cast_shadow");

    ShaderParam *cascade_map = m_deferred_shader->GetParam("u_cascade_map");
    ShaderParam *num_of_cascades =
        m_deferred_shader->GetParam("u_num_of_cascades");
    ShaderParam *cascade_planes =
        m_deferred_shader->GetParam("u_cascade_planes[0]");

    RenderOperation op;
    op.blend = false;

    const uint8_t input_id = 0;
    const uint8_t output_id = 1;
    // clear the last lighting pass' result
    float value[]{0, 0, 0, 0};
    m_light_buffer[input_id]->ClearAttachment(0, value);
    lightView.each([&](const TransformComponent &transformComp,
                       LightComponent &lightComp) {
        Light &light = lightComp.light;
        CascadeShadow &shadow = lightComp.shadow;
        const Transform &transform = transformComp.GetWorldTransform();

        RenderPassInfo info{m_light_buffer[output_id].get(),
                            m_light_buffer[output_id]->GetWidth(),
                            m_light_buffer[output_id]->GetHeight(),
                            op,
                            BufferBitMask::ColorBufferBit,
                            {0, 0, 0, 0}};

        Renderer::BeginRenderPass(info);

        Renderer::SetCamera(*m_deferred_shader, GetCamera());
        lighting->SetAsTexture(m_light_buffer[input_id]->GetTexture());
        direction->SetAsVec3(&transform.GetFront()[0]);
        position->SetAsVec3(&transform.GetPosition()[0]);
        ambient->SetAsVec3(&light.GetAmbient()[0]);
        diffuse->SetAsVec3(&light.GetDiffuse()[0]);
        specular->SetAsVec3(&light.GetSpecular()[0]);
        cutoff->SetAsFloat(glm::cos(light.GetCutoff()));
        outer_cutoff->SetAsFloat(glm::cos(light.GetOuterCutoff()));
        constant->SetAsFloat(light.GetConstant());
        linear->SetAsFloat(light.GetLinear());
        quadratic->SetAsFloat(light.GetQuadratic());

        is_directional->SetAsBool(light.IsDirectional());
        is_cast_shadow->SetAsBool(light.IsCastShadow());
        if (light.IsCastShadow()) {
            RenderShadowMap(shadow, transform);

            cascade_map->SetAsTexture(shadow.GetCascadeMap()->GetTexture());
            auto &planes = shadow.GetCascadePlanes();
            num_of_cascades->SetAsInt(planes.size());
            cascade_planes->SetAsVec(&planes[0], planes.size());
            Renderer3D::SetShadowCaster(*m_deferred_shader, shadow);
        }
        Renderer::DrawNDCQuad(*m_deferred_shader);
        std::swap(m_light_buffer[input_id], m_light_buffer[output_id]);
        Renderer::EndRenderPass();
    });
}

void LightingSystem::RenderGBuffer()
{
    auto modelView = GetScene().view<TransformComponent, ModelComponent>();

    RenderPassInfo info;
    info.framebuffer = m_gbuffer.get();
    info.viewport_width = m_gbuffer->GetWidth();
    info.viewport_height = m_gbuffer->GetHeight();
    info.clear_mask =
        BufferBitMask::ColorBufferBit | BufferBitMask::DepthBufferBit;
    info.op.blend = false;
    Renderer::BeginRenderPass(info);
    Renderer::SetCamera(*m_gbuffer_shader, GetCamera());
    uint32_t id = static_cast<uint32_t>(entt::null);
    m_gbuffer->ClearAttachment(static_cast<int>(GeometryBufferType::EntityId),
                               &id);

    auto &storage = AssetStorage::Get();
    ShaderParam *entity_id = m_gbuffer_shader->GetParam("u_entity_id");
    ShaderParam *base_color = m_gbuffer_shader->GetParam("u_color");
    ShaderParam *model_param = m_gbuffer_shader->GetParam("u_model");
    modelView.each([&](const entt::entity &entity,
                       const TransformComponent &transformComp,
                       const ModelComponent &modelComp) {
        entity_id->SetAsUint(static_cast<uint32_t>(entity));
        base_color->SetAsVec3(&modelComp.color[0]);
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

void LightingSystem::SetSSAORadius(float radius) { m_ssao_radius = radius; }

void LightingSystem::SetSSAOBias(float bias) { m_ssao_bias = bias; }

}  // namespace SD
