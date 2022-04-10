#include "Renderer/DeferredRenderer.hpp"
#include "Core/Application.hpp"
#include "Renderer/Renderer3D.hpp"
#include "ECS/Component.hpp"

#include "Utility/Random.hpp"

#include "ImGui/ImGuiWidget.hpp"

#include "Loader/ShaderLoader.hpp"

#include "Core/Application.hpp"

namespace SD {

struct DeferredRenderData {
    CascadeShadow cascade_shadow;
    Ref<Shader> cascade_shader;
    Ref<Shader> cascade_debug_shader;
    Ref<Framebuffer> cascade_debug_target;
    Ref<Texture> cascade_debug_buffer;
    int debug_layer{0};

    PointShadow point_shadow;
    Ref<Shader> point_shadow_shader;

    Ref<Shader> emssive_shader;

    Ref<Shader> deferred_shader;
    Ref<Framebuffer> lighting_target[2];
    Ref<Texture> lighting_buffers[2];
    Texture *lighting_result;

    Ref<Shader> gbuffer_shader;
    Ref<Framebuffer> geometry_target_msaa;
    std::array<Ref<Texture>, static_cast<int>(GeometryBufferType::GBufferCount)>
        gbuffer_msaa;
    Ref<Framebuffer> geometry_target;
    std::array<Ref<Texture>, static_cast<int>(GeometryBufferType::GBufferCount)>
        gbuffer;
    Ref<Renderbuffer> depth_buffer;

    Ref<Shader> ssao_shader;
    Ref<Texture> ssao_buffer;
    Ref<Shader> ssao_blur_shader;
    Ref<Texture> ssao_blur_buffer;

    Ref<Texture> ssao_noise;
    std::vector<Vector3f> ssao_kernel;
};

static DeferredRenderData s_data;
static DeferredRenderSettings s_settings;

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

void DeferredRenderer::Init(const DeferredRenderSettings &settings)
{
    s_settings = settings;
    s_data.cascade_shadow.CreateShadowMap(4096, 4096);
    s_data.point_shadow.CreateShadowMap(4096, 4096);
    for (int i = 0; i < 2; ++i) {
        s_data.lighting_target[i] = Framebuffer::Create();
    }
    s_data.geometry_target_msaa = Framebuffer::Create();
    s_data.geometry_target = Framebuffer::Create();
    s_data.cascade_debug_target = Framebuffer::Create();
    InitShaders();
    InitSSAOKernel();
    InitSSAOBuffers();
    InitLightingBuffers();
}

void DeferredRenderer::InitShaders()
{
    s_data.emssive_shader = ShaderLoader::LoadShader(
        "assets/shaders/quad.vert.glsl", "assets/shaders/emissive.frag.glsl");
    s_data.deferred_shader = ShaderLoader::LoadShader(
        "assets/shaders/quad.vert.glsl", "assets/shaders/deferred.frag.glsl");
    s_data.gbuffer_shader = ShaderLoader::LoadShader(
        "assets/shaders/mesh.vert.glsl", "assets/shaders/gbuffer.frag.glsl");

    s_data.ssao_shader =
        ShaderLoader::LoadShader("assets/shaders/ssao.comp.glsl");
    s_data.ssao_blur_shader =
        ShaderLoader::LoadShader("assets/shaders/ssao_blur.comp.glsl");

    s_data.cascade_shader =
        ShaderLoader::LoadShader("assets/shaders/shadow.vert.glsl", "",
                                 "assets/shaders/shadow.geo.glsl");
    s_data.cascade_debug_shader =
        ShaderLoader::LoadShader("assets/shaders/quad.vert.glsl",
                                 "assets/shaders/debug_depth.frag.glsl");
    s_data.point_shadow_shader =
        ShaderLoader::LoadShader("assets/shaders/shadow.vert.glsl",
                                 "assets/shaders/point_shadow.frag.glsl",
                                 "assets/shaders/point_shadow.geo.glsl");
}

void DeferredRenderer::InitSSAOBuffers()
{
    s_data.ssao_buffer = Texture::Create(s_settings.width, s_settings.height, 0,
                                         MultiSampleLevel::None,
                                         TextureType::Normal, DataFormat::R16F);
    s_data.ssao_blur_buffer = Texture::Create(
        s_settings.width, s_settings.height, 0, MultiSampleLevel::None,
        TextureType::Normal, DataFormat::R16F);
}

void DeferredRenderer::InitSSAOKernel()
{
    uint32_t kernel_size = s_data.ssao_shader->GetUint("u_kernel_size");
    s_data.ssao_kernel.resize(kernel_size);
    for (uint32_t i = 0; i < kernel_size; ++i) {
        Vector3f sample(Random::Rnd(-1.f, 1.f), Random::Rnd(-1.f, 1.f),
                        Random::Rnd(0.f, 1.0f));
        sample = glm::normalize(sample);
        sample *= Random::Rnd(0.f, 1.0f);
        float scale = static_cast<float>(i) / kernel_size;

        // scale samples s.t. they're more aligned to center of kernel
        scale = Math::Lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        s_data.ssao_kernel[i] = (sample);
    }

    std::array<Vector3f, 16> ssao_noise;
    for (uint32_t i = 0; i < 16; i++) {
        Vector3f noise(Random::Rnd(-1.f, 1.0f), Random::Rnd(-1.f, 1.0f),
                       0.0f);  // rotate around z-axis (in tangent space)
        ssao_noise[i] = glm::normalize(noise);
    }
    s_data.ssao_noise =
        Texture::Create(4, 4, 0, MultiSampleLevel::None, TextureType::Normal,
                        DataFormat::RGB16F, TextureWrap::Repeat);
    s_data.ssao_noise->SetPixels(0, 0, 0, 4, 4, 1, ssao_noise.data());

    s_data.ssao_shader->GetParam("u_samples[0]")
        ->SetAsVec3(&s_data.ssao_kernel[0][0], kernel_size);
}

void DeferredRenderer::InitLightingBuffers()
{
    for (int i = 0; i < 2; ++i) {
        s_data.lighting_buffers[i] = Texture::Create(
            s_settings.width, s_settings.height, 0, s_settings.msaa,
            TextureType::Normal, DataFormat::RGB16F);
        s_data.lighting_target[i]->Attach(*s_data.lighting_buffers[i], 0, 0);
    }

    for (size_t i = 0; i < s_data.gbuffer_msaa.size(); ++i) {
        s_data.gbuffer_msaa[i] = Texture::Create(
            s_settings.width, s_settings.height, 0, s_settings.msaa,
            TextureType::Normal, GetTextureFormat(GeometryBufferType(i)));
        s_data.geometry_target_msaa->Attach(*s_data.gbuffer_msaa[i], i, 0);

        s_data.gbuffer[i] = Texture::Create(
            s_settings.width, s_settings.height, 0, MultiSampleLevel::None,
            TextureType::Normal, GetTextureFormat(GeometryBufferType(i)));
        s_data.geometry_target->Attach(*s_data.gbuffer[i], i, 0);
    }
    s_data.depth_buffer =
        Renderbuffer::Create(s_settings.width, s_settings.height,
                             s_settings.msaa, DataFormat::Depth24);
    s_data.geometry_target_msaa->Attach(*s_data.depth_buffer, 0);

    s_data.cascade_debug_buffer = Texture::Create(
        s_settings.width, s_settings.height, 0, MultiSampleLevel::None,
        TextureType::Normal, DataFormat::RGB16F);
    s_data.cascade_debug_target->Attach(*s_data.cascade_debug_buffer, 0, 0);
}

void DeferredRenderer::SetRenderSize(int32_t width, int32_t height)
{
    s_settings.width = width;
    s_settings.height = height;
    InitSSAOBuffers();
    InitLightingBuffers();
}

void DeferredRenderer::BlitGeometryBuffers()
{
    Renderer::BeginRenderPass(
        {s_data.geometry_target.get(), s_settings.width, s_settings.height});
    for (size_t i = 0; i < s_data.gbuffer.size(); ++i) {
        Renderer::BlitFromBuffer(i, s_data.geometry_target_msaa.get(), i,
                                 BufferBitMask::ColorBufferBit);
    }
    Renderer::EndRenderPass();
}

void DeferredRenderer::ImGui()
{
    if (ImGui::TreeNodeEx("Geometry Buffers")) {
        for (size_t i = 0; i < s_data.gbuffer.size(); ++i) {
            ImGui::DrawTexture(*s_data.gbuffer[i], ImVec2(0, 1), ImVec2(1, 0));
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("SSAO")) {
        ImGui::Checkbox("On", &s_settings.ssao_state);
        ImGui::TextUnformatted("SSAO Power");
        ImGui::SliderInt("##SSAO Power", &s_settings.ssao_power, 1, 32);
        ImGui::TextUnformatted("SSAO Radius");
        ImGui::SliderFloat("##SSAO Radius", &s_settings.ssao_radius, 0.1, 30);
        ImGui::TextUnformatted("SSAO Bias");
        ImGui::SliderFloat("##SSAO Bias", &s_settings.ssao_bias, 0.01, 2);
        ImGui::DrawTexture(*s_data.ssao_blur_buffer, ImVec2(0, 1),
                           ImVec2(1, 0));
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Cascade Shadow")) {
        auto planes = s_data.cascade_shadow.GetCascadePlanes();
        int num_of_cascades = planes.size();
        if (ImGui::SliderInt("Num of Cascades", &num_of_cascades, 1, 4)) {
            s_data.cascade_shadow.SetNumOfCascades(num_of_cascades);
        }
        if (ImGui::InputFloat4("Cascades", planes.data())) {
            s_data.cascade_shadow.SetCascadePlanes(planes);
        }
        ImGui::InputInt("Layer", &s_data.debug_layer);
        ImGui::DrawTexture(*s_data.cascade_debug_buffer, ImVec2(0, 1),
                           ImVec2(1, 0));
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Point Shadow")) {
        float far_z = s_data.point_shadow.GetFarZ();
        if (ImGui::SliderFloat("Shadow Far Z", &far_z, 1.0f, 500.f)) {
            s_data.point_shadow.SetFarZ(far_z);
        }
        ImGui::TreePop();
    }
}

void DeferredRenderer::Render(const Scene &scene, const Camera &camera)
{
    RenderGBuffer(scene);
    BlitGeometryBuffers();

    if (s_settings.ssao_state) {
        RenderSSAO();
    }

    RenderDeferred(scene, camera);
    auto dir_lights =
        scene.view<TransformComponent, DirectionalLightComponent>();
    auto point_lights = scene.view<TransformComponent, PointLightComponent>();
    if (dir_lights.begin() != dir_lights.end() ||
        point_lights.begin() != point_lights.end()) {
        RenderEmissive();
    }
    Renderer::BlitFromBuffer(
        1, s_data.geometry_target_msaa.get(),
        static_cast<int>(GeometryBufferType::EntityId),
        BufferBitMask::ColorBufferBit | BufferBitMask::DepthBufferBit);
}

void DeferredRenderer::RenderShadowMap(const Scene &scene, const Camera &camera,
                                       const Transform &transform)
{
    auto modelView = scene.view<TransformComponent, MeshComponent>();
    RenderOperation op;
    op.cull_face = Face::Front;
    Texture *depth_map = s_data.cascade_shadow.GetShadowMap();
    Framebuffer *target = s_data.cascade_shadow.GetShadowTarget();
    Renderer::BeginRenderPass(RenderPassInfo{target, depth_map->GetWidth(),
                                             depth_map->GetHeight(), op});
    s_data.cascade_shadow.ComputeCascadeLightMatrix(transform, camera);
    Renderer3D::BindCascadeShadow(*s_data.cascade_shader);
    Renderer3D::SetCascadeShadow(s_data.cascade_shadow);

    ShaderParam *model_param = s_data.cascade_shader->GetParam("u_model");
    modelView.each(
        [&](const TransformComponent &tc, const MeshComponent &mc) {
            Matrix4f mat = tc.GetWorldTransform().GetMatrix();
            model_param->SetAsMat4(&mat[0][0]);
            Renderer3D::DrawMesh(*s_data.cascade_shader, *mc.mesh);
        });
    Renderer::EndRenderPass();

    // debug
    {
        Renderer::BeginRenderPass(RenderPassInfo{
            s_data.cascade_debug_target.get(),
            s_data.cascade_debug_buffer->GetWidth(),
            s_data.cascade_debug_buffer->GetHeight(), RenderOperation{}});
        s_data.cascade_debug_shader->GetParam("depthMap")
            ->SetAsTexture(depth_map);
        s_data.cascade_debug_shader->GetParam("layer")->SetAsInt(
            s_data.debug_layer);
        s_data.cascade_debug_shader->GetParam("near_plane")
            ->SetAsFloat(camera.GetNearZ());
        s_data.cascade_debug_shader->GetParam("far_plane")
            ->SetAsFloat(camera.GetFarZ());
        Renderer::DrawNDCQuad(*s_data.cascade_debug_shader);

        Renderer::EndRenderPass();
    }
}

void DeferredRenderer::RenderPointShadowMap(const Scene &scene,
                                            const Transform &transform)
{
    Vector3f light_pos = transform.GetPosition();
    std::array<Matrix4f, 6> shadow_trans =
        s_data.point_shadow.GetProjectionMatrix(light_pos);

    auto modelView = scene.view<TransformComponent, MeshComponent>();
    RenderOperation op;
    op.cull_face = Face::Front;
    Texture *shadow_map = s_data.point_shadow.GetShadowMap();
    Framebuffer *shadow_target = s_data.point_shadow.GetShadowTarget();
    Renderer::BeginRenderPass(RenderPassInfo{
        shadow_target, shadow_map->GetWidth(), shadow_map->GetHeight(), op});

    ShaderParam *model_param = s_data.point_shadow_shader->GetParam("u_model");
    s_data.point_shadow_shader->GetParam("u_light_pos")
        ->SetAsVec3(&transform.GetPosition()[0]);
    s_data.point_shadow_shader->GetParam("u_far_z")->SetAsFloat(
        s_data.point_shadow.GetFarZ());
    s_data.point_shadow_shader->GetParam("u_shadow_matrix[0]")
        ->SetAsMat4(&shadow_trans[0][0][0], 6);
    modelView.each(
        [&](const TransformComponent &tc, const MeshComponent &mc) {
            Matrix4f mat = tc.GetWorldTransform().GetMatrix();
            model_param->SetAsMat4(&mat[0][0]);
            Renderer3D::DrawMesh(*s_data.point_shadow_shader, *mc.mesh);
        });
    Renderer::EndRenderPass();
}

void DeferredRenderer::RenderSSAO()
{
    Texture *position =
        s_data.gbuffer[static_cast<int>(GeometryBufferType::Position)].get();
    Texture *normal =
        s_data.gbuffer[static_cast<int>(GeometryBufferType::Normal)].get();
    Renderer::BindCamera(*s_data.ssao_shader);
    s_data.ssao_shader->GetParam("u_radius")
        ->SetAsFloat(s_settings.ssao_radius);
    s_data.ssao_shader->GetParam("u_bias")->SetAsFloat(s_settings.ssao_bias);
    s_data.ssao_shader->GetParam("u_power")->SetAsUint(s_settings.ssao_power);
    s_data.ssao_shader->GetParam("u_position")->SetAsTexture(position);
    s_data.ssao_shader->GetParam("u_normal")->SetAsTexture(normal);
    s_data.ssao_shader->GetParam("u_noise")->SetAsTexture(
        s_data.ssao_noise.get());
    s_data.ssao_shader->GetParam("u_out_image")
        ->SetAsImage(s_data.ssao_buffer.get(), 0, false, 0, Access::WriteOnly);
    Renderer::ComputeImage(*s_data.ssao_shader, s_settings.width,
                           s_settings.height, 1);

    // blur
    s_data.ssao_blur_shader->GetParam("u_input")->SetAsTexture(
        s_data.ssao_buffer.get());
    s_data.ssao_blur_shader->GetParam("u_out_image")
        ->SetAsImage(s_data.ssao_blur_buffer.get(), 0, false, 0,
                     Access::WriteOnly);
    Renderer::ComputeImage(*s_data.ssao_blur_shader, s_settings.width,
                           s_settings.height, 1);
}

void DeferredRenderer::RenderEmissive()
{
    RenderOperation op;
    op.blend = false;
    const int buffer = 0;
    Renderer::BeginRenderSubpass(RenderSubpassInfo{&buffer, 1, op});
    s_data.emssive_shader->GetParam("u_lighting")
        ->SetAsTexture(s_data.lighting_result);
    s_data.emssive_shader->GetParam("u_emissive")
        ->SetAsTexture(
            s_data.gbuffer_msaa[static_cast<int>(GeometryBufferType::Emissive)]
                .get());
    Renderer::DrawNDCQuad(*s_data.emssive_shader);
    Renderer::EndRenderSubpass();
}

void DeferredRenderer::RenderDeferred(const Scene &scene, const Camera &camera)
{
    s_data.deferred_shader->GetParam("u_position")
        ->SetAsTexture(
            s_data.gbuffer_msaa[static_cast<int>(GeometryBufferType::Position)]
                .get());
    s_data.deferred_shader->GetParam("u_normal")
        ->SetAsTexture(
            s_data.gbuffer_msaa[static_cast<int>(GeometryBufferType::Normal)]
                .get());
    s_data.deferred_shader->GetParam("u_albedo")
        ->SetAsTexture(
            s_data.gbuffer_msaa[static_cast<int>(GeometryBufferType::Albedo)]
                .get());
    s_data.deferred_shader->GetParam("u_ambient")
        ->SetAsTexture(
            s_data.gbuffer_msaa[static_cast<int>(GeometryBufferType::Ambient)]
                .get());
    s_data.deferred_shader->GetParam("u_background")
        ->SetAsTexture(
            Renderer::GetCurrentRenderPass().framebuffer->GetAttachment(0));
    s_data.deferred_shader->GetParam("u_ssao")->SetAsTexture(
        s_data.ssao_blur_buffer.get());
    s_data.deferred_shader->GetParam("u_ssao_state")
        ->SetAsBool(s_settings.ssao_state);

    ShaderParam *lighting = s_data.deferred_shader->GetParam("u_lighting");
    ShaderParam *ambient =
        s_data.deferred_shader->GetParam("u_light_color.ambient");
    ShaderParam *diffuse =
        s_data.deferred_shader->GetParam("u_light_color.diffuse");
    ShaderParam *specular =
        s_data.deferred_shader->GetParam("u_light_color.specular");
    ShaderParam *light_front =
        s_data.deferred_shader->GetParam("u_light_front");
    ShaderParam *is_directional =
        s_data.deferred_shader->GetParam("u_is_directional");
    ShaderParam *is_cast_shadow =
        s_data.deferred_shader->GetParam("u_is_cast_shadow");

    ShaderParam *cascade_map =
        s_data.deferred_shader->GetParam("u_cascade_map");
    ShaderParam *num_of_cascades =
        s_data.deferred_shader->GetParam("u_num_of_cascades");
    ShaderParam *cascade_planes =
        s_data.deferred_shader->GetParam("u_cascade_planes[0]");

    Renderer::BindCamera(*s_data.deferred_shader);
    Renderer3D::BindCascadeShadow(*s_data.deferred_shader);

    RenderOperation op;
    op.blend = false;

    uint8_t input_id = 0;
    uint8_t output_id = 1;
    // clear the last lighting pass' result
    const float value[]{0, 0, 0};
    s_data.lighting_target[input_id]->ClearAttachment(0, value);
    auto dir_lights =
        scene.view<TransformComponent, DirectionalLightComponent>();
    dir_lights.each([&](const TransformComponent &transformComp,
                        const DirectionalLightComponent &lightComp) {
        const DirectionalLight &light = lightComp.light;
        const Transform &transform = transformComp.GetWorldTransform();

        RenderPassInfo info{s_data.lighting_target[output_id].get(),
                            s_data.lighting_buffers[output_id]->GetWidth(),
                            s_data.lighting_buffers[output_id]->GetHeight(),
                            op,
                            BufferBitMask::ColorBufferBit,
                            {0, 0, 0, 0}};
        s_data.lighting_result = s_data.lighting_buffers[output_id].get();

        Renderer::BeginRenderPass(info);

        lighting->SetAsTexture(s_data.lighting_buffers[input_id].get());
        light_front->SetAsVec3(&transform.GetFront()[0]);
        ambient->SetAsVec3(&light.ambient[0]);
        diffuse->SetAsVec3(&light.diffuse[0]);
        specular->SetAsVec3(&light.specular[0]);

        is_directional->SetAsBool(true);
        is_cast_shadow->SetAsBool(lightComp.is_cast_shadow);
        if (lightComp.is_cast_shadow) {
            RenderShadowMap(scene, camera, transform);

            cascade_map->SetAsTexture(s_data.cascade_shadow.GetShadowMap());
            auto &planes = s_data.cascade_shadow.GetCascadePlanes();
            num_of_cascades->SetAsInt(planes.size());
            cascade_planes->SetAsVec(&planes[0], planes.size());
        }

        Renderer::DrawNDCQuad(*s_data.deferred_shader);
        std::swap(input_id, output_id);
        Renderer::EndRenderPass();
    });

    ShaderParam *light_position =
        s_data.deferred_shader->GetParam("u_point_light.pos");
    ShaderParam *constant =
        s_data.deferred_shader->GetParam("u_point_light.constant");
    ShaderParam *linear =
        s_data.deferred_shader->GetParam("u_point_light.linear");
    ShaderParam *quadratic =
        s_data.deferred_shader->GetParam("u_point_light.quadratic");
    ShaderParam *point_shadow_map =
        s_data.deferred_shader->GetParam("u_point_shadow_map");
    ShaderParam *point_shadow_far_z =
        s_data.deferred_shader->GetParam("u_point_shadow_far_z");
    auto point_lights = scene.view<TransformComponent, PointLightComponent>();
    point_lights.each([&](const TransformComponent &transformComp,
                          const PointLightComponent &lightComp) {
        const PointLight &light = lightComp.light;
        const Transform &transform = transformComp.GetWorldTransform();

        RenderPassInfo info{s_data.lighting_target[output_id].get(),
                            s_data.lighting_buffers[output_id]->GetWidth(),
                            s_data.lighting_buffers[output_id]->GetHeight(),
                            op,
                            BufferBitMask::ColorBufferBit,
                            {0, 0, 0, 0}};
        s_data.lighting_result = s_data.lighting_buffers[output_id].get();

        Renderer::BeginRenderPass(info);

        lighting->SetAsTexture(s_data.lighting_buffers[input_id].get());
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
            RenderPointShadowMap(scene, transform);

            point_shadow_map->SetAsTexture(s_data.point_shadow.GetShadowMap());
            point_shadow_far_z->SetAsFloat(s_data.point_shadow.GetFarZ());
        }

        Renderer::DrawNDCQuad(*s_data.deferred_shader);
        std::swap(input_id, output_id);
        Renderer::EndRenderPass();
    });
}

void DeferredRenderer::RenderGBuffer(const Scene &scene)
{
    auto meshes =
        scene.view<TransformComponent, MeshComponent, MaterialComponent>();

    RenderPassInfo info;
    info.framebuffer = s_data.geometry_target_msaa.get();
    info.viewport_width = s_settings.width;
    info.viewport_height = s_settings.height;
    info.clear_mask =
        BufferBitMask::ColorBufferBit | BufferBitMask::DepthBufferBit;
    info.clear_value = {0, 0, 0, 0};
    info.op.blend = false;
    Renderer::BeginRenderPass(info);
    Renderer::BindCamera(*s_data.gbuffer_shader);
    uint32_t id = static_cast<uint32_t>(entt::null);
    s_data.geometry_target_msaa->ClearAttachment(
        static_cast<int>(GeometryBufferType::EntityId), &id);

    ShaderParam *entity_id = s_data.gbuffer_shader->GetParam("u_entity_id");
    ShaderParam *model_param = s_data.gbuffer_shader->GetParam("u_model");
    meshes.each(
        [&](const entt::entity &entity, const TransformComponent &transform,
            const MeshComponent &mesh, const MaterialComponent &material) {
            entity_id->SetAsUint(static_cast<uint32_t>(entity));
            Renderer3D::SetMaterial(*s_data.gbuffer_shader, material.material);
            Matrix4f mat = transform.GetWorldTransform().GetMatrix();
            model_param->SetAsMat4(&mat[0][0]);
            Renderer3D::DrawMesh(*s_data.gbuffer_shader, *mesh.mesh);
        });
    Renderer::EndRenderPass();
}

}  // namespace SD
