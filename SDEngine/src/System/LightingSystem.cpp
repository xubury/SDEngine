#include "System/LightingSystem.hpp"
#include "Asset/AssetManager.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Component.hpp"
#include "Utility/Random.hpp"
#include "ImGui/ImGuiWidget.hpp"

namespace SD {

DataFormat GetTextureFormat(GeometryBufferType type) {
    switch (type) {
        case GeometryBufferType::G_ALBEDO:
            return DataFormat::RGBA;
        case GeometryBufferType::G_POSITION:
        case GeometryBufferType::G_NORMAL:
        case GeometryBufferType::G_AMBIENT:
        case GeometryBufferType::G_EMISSIVE:
            return DataFormat::RGB;
        case GeometryBufferType::G_ENTITY_ID:
            return DataFormat::RED;
        default:
            SD_CORE_WARN("Unknown GBuffer!");
            return DataFormat::RGBA;
    }
}

DataFormatType GetTextureFormatType(GeometryBufferType type) {
    switch (type) {
        case GeometryBufferType::G_POSITION:
        case GeometryBufferType::G_NORMAL:
            return DataFormatType::FLOAT16;
        case GeometryBufferType::G_ALBEDO:
        case GeometryBufferType::G_AMBIENT:
        case GeometryBufferType::G_EMISSIVE:
            return DataFormatType::UBYTE;
        case GeometryBufferType::G_ENTITY_ID:
            return DataFormatType::UINT;
        default:
            SD_CORE_WARN("Unknown GBuffer!");
            return DataFormatType::UBYTE;
    }
}

LightingSystem::LightingSystem(int width, int height)
    : System("LightingSystem"), m_width(width), m_height(height) {
    const float quadVertices[] = {
        -1.0f, -1.0f, 0.f, 0.f,  0.f,   // bottom left
        1.0f,  -1.0f, 0.f, 1.0f, 0.f,   // bottom right
        1.0f,  1.0f,  0.f, 1.0f, 1.0f,  // top right
        -1.0f, 1.0f,  0.f, 0.f,  1.0f,  // top left
    };
    const uint32_t indices[] = {0, 1, 2, 2, 3, 0};
    auto buffer = VertexBuffer::Create(quadVertices, sizeof(quadVertices),
                                       BufferIOType::STATIC);
    auto indexBuffer = IndexBuffer::Create(indices, 6, BufferIOType::STATIC);
    m_quad = VertexArray::Create();
    VertexBufferLayout layout;
    layout.Push(BufferLayoutType::FLOAT3);
    layout.Push(BufferLayoutType::FLOAT2);
    m_quad->AddVertexBuffer(buffer, layout);
    m_quad->SetIndexBuffer(indexBuffer);
}

void LightingSystem::OnInit() {
    InitShaders();
    InitSSAOKernel();
    InitSSAO();
    InitLighting();
}

void LightingSystem::OnPush() {
    m_size_handler = dispatcher->Register(this, &LightingSystem::OnSizeEvent);

    m_ssao_state = ini->GetBoolean("ssao", "state", true);
    m_ssao_radius = ini->GetFloat("ssao", "radius", 0.5);
    m_ssao_bias = ini->GetFloat("ssao", "bias", 0.25);
    m_ssao_power = ini->GetInteger("ssao", "power", 1);
}

void LightingSystem::OnPop() {
    dispatcher->RemoveHandler(m_size_handler);

    ini->SetBoolean("ssao", "state", m_ssao_state);
    ini->SetFloat("ssao", "radius", m_ssao_radius);
    ini->SetFloat("ssao", "bias", m_ssao_bias);
    ini->SetInteger("ssao", "power", m_ssao_power);
}

void LightingSystem::InitShaders() {
    m_shadow_shader = asset->LoadAndGet<Shader>("shaders/shadow.glsl");
    m_emssive_shader = asset->LoadAndGet<Shader>("shaders/emissive.glsl");
    m_deferred_shader = asset->LoadAndGet<Shader>("shaders/deferred.glsl");
    m_gbuffer_shader = asset->LoadAndGet<Shader>("shaders/gbuffer.glsl");
    m_ssao_shader = asset->LoadAndGet<Shader>("shaders/ssao.glsl");
    m_ssao_blur_shader = asset->LoadAndGet<Shader>("shaders/ssao_blur.glsl");
}

void LightingSystem::InitSSAO() {
    // ssao target
    TextureSpec spec(1, TextureType::TEX_2D, DataFormat::RED,
                     DataFormatType::FLOAT16, TextureWrap::EDGE,
                     TextureMagFilter::NEAREST, TextureMinFilter::NEAREST);

    m_ssao_buffer = Framebuffer::Create(m_width, m_height);
    m_ssao_buffer->Attach(spec);

    m_ssao_blur_buffer = Framebuffer::Create(m_width, m_height);
    m_ssao_blur_buffer->Attach(spec);
}

void LightingSystem::InitSSAOKernel() {
    uint32_t kernel_size = m_ssao_shader->GetUint("u_kernel_size");
    m_ssao_kernel.resize(kernel_size);
    for (uint32_t i = 0; i < kernel_size; ++i) {
        glm::vec3 sample(Random::Rnd(-1.f, 1.f), Random::Rnd(-1.f, 1.f),
                         Random::Rnd(0.f, 1.0f));
        sample = glm::normalize(sample);
        sample *= Random::Rnd(0.f, 1.0f);
        float scale = static_cast<float>(i) / kernel_size;

        // scale samples s.t. they're more aligned to center of kernel
        scale = Math::Lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        m_ssao_kernel[i] = (sample);
    }

    std::array<glm::vec3, 16> ssao_noise;
    for (uint32_t i = 0; i < 16; i++) {
        glm::vec3 noise(Random::Rnd(-1.f, 1.0f), Random::Rnd(-1.f, 1.0f),
                        0.0f);  // rotate around z-axis (in tangent space)
        ssao_noise[i] = glm::normalize(noise);
    }
    m_ssao_noise = Texture::Create(
        4, 4,
        TextureSpec(1, TextureType::TEX_2D, DataFormat::RGB,
                    DataFormatType::FLOAT16, TextureWrap::REPEAT,
                    TextureMagFilter::NEAREST, TextureMinFilter::NEAREST));
    m_ssao_noise->SetPixels(0, 0, 0, 4, 4, 1, ssao_noise.data());

    for (uint32_t i = 0; i < kernel_size; ++i) {
        m_ssao_shader->SetVec3("u_samples[" + std::to_string(i) + "]",
                               m_ssao_kernel[i]);
    }
}

void LightingSystem::InitLighting() {
    int samples = std::max<int>(device->GetMSAA(), 1);

    for (int i = 0; i < 2; ++i) {
        m_light_buffer[i] = Framebuffer::Create(m_width, m_height);
        m_light_buffer[i]->Attach(TextureSpec(
            samples, TextureType::TEX_2D_MULTISAMPLE, DataFormat::RGB,
            DataFormatType::FLOAT16, TextureWrap::EDGE,
            TextureMagFilter::NEAREST, TextureMinFilter::NEAREST));
    }

    m_gbuffer = Framebuffer::Create(m_width, m_height);
    for (int i = 0; i < GeometryBufferType::GBUFFER_COUNT; ++i) {
        m_gbuffer->Attach(TextureSpec(
            samples, TextureType::TEX_2D_MULTISAMPLE,
            GetTextureFormat(GeometryBufferType(i)),
            GetTextureFormatType(GeometryBufferType(i)), TextureWrap::EDGE,
            TextureMagFilter::NEAREST, TextureMinFilter::NEAREST));
    }
    m_gbuffer->Attach(
        RenderbufferSpec(samples, DataFormat::DEPTH, DataFormatType::FLOAT16));
}

void LightingSystem::OnSizeEvent(const ViewportEvent &event) {
    m_width = event.width;
    m_height = event.height;
    InitLighting();
    InitSSAO();
}

void LightingSystem::OnImGui() {
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
}

void LightingSystem::OnRender() {
    SD_CORE_ASSERT(scene->GetCamera(), "No camera is set!");

    RenderShadowMap();
    device->Disable(Operation::BLEND);
    RenderGBuffer();
    if (m_ssao_state) {
        RenderSSAO();
    }
    RenderDeferred();
    device->Enable(Operation::BLEND);
    RenderEmissive();

    device->ReadBuffer(m_gbuffer.get(), G_ENTITY_ID);
    device->DrawBuffer(renderer->GetFramebuffer(), 1);
    device->BlitFramebuffer(
        m_gbuffer.get(), 0, 0, m_gbuffer->GetWidth(), m_gbuffer->GetHeight(),
        renderer->GetFramebuffer(), 0, 0,
        renderer->GetFramebuffer()->GetWidth(),
        renderer->GetFramebuffer()->GetHeight(),
        BufferBitMask::COLOR_BUFFER_BIT | BufferBitMask::DEPTH_BUFFER_BIT,
        BlitFilter::NEAREST);
}

void LightingSystem::RenderShadowMap() {
    auto lightView = scene->view<TransformComponent, LightComponent>();
    auto modelView = scene->view<TransformComponent, ModelComponent>();
    device->SetCullFace(Face::FRONT);
    lightView.each([this, &modelView](const TransformComponent &transformComp,
                                      LightComponent &lightComp) {
        Light &light = lightComp.light;
        if (!light.IsCastShadow()) return;

        device->SetFramebuffer(light.GetShadowMap());
        device->SetShader(nullptr);  // supress shader recompile warning
        light.GetShadowMap()->ClearDepth();
        light.ComputeLightSpaceMatrix(transformComp.GetWorldTransform(),
                                      scene->GetCamera());
        m_shadow_shader->SetMat4("u_projection_view",
                                 light.GetProjectionView());

        modelView.each([this](const TransformComponent &transformComp,
                              const ModelComponent &modelComp) {
            auto model = asset->Get<Model>(modelComp.id);
            m_shadow_shader->SetMat4(
                "u_model", transformComp.GetWorldTransform().GetMatrix());
            if (model) {
                for (const auto &mesh : model->GetMeshes()) {
                    renderer->DrawMesh(*m_shadow_shader, mesh);
                }
            }
        });
    });
    device->SetCullFace(Face::BACK);
    device->SetShader(nullptr);  // supress shader recompile warning
}

void LightingSystem::RenderSSAO() {
    const float CLEAR_VALUE = 1.0f;

    device->SetFramebuffer(m_ssao_buffer.get());
    m_ssao_buffer->ClearAttachment(0, &CLEAR_VALUE);
    renderer->Begin(*m_ssao_shader, *scene->GetCamera());
    m_ssao_shader->SetFloat("u_radius", m_ssao_radius);
    m_ssao_shader->SetFloat("u_bias", m_ssao_bias);
    m_ssao_shader->SetUint("u_power", m_ssao_power);
    m_ssao_shader->SetTexture(
        "u_position", m_gbuffer->GetTexture(GeometryBufferType::G_POSITION));
    m_ssao_shader->SetTexture(
        "u_normal", m_gbuffer->GetTexture(GeometryBufferType::G_NORMAL));
    m_ssao_shader->SetTexture("u_noise", m_ssao_noise.get());
    renderer->Submit(*m_ssao_shader, *m_quad, MeshTopology::TRIANGLES,
                     m_quad->GetIndexBuffer()->GetCount(), 0);
    renderer->End();

    // blur
    device->SetFramebuffer(m_ssao_blur_buffer.get());
    m_ssao_blur_buffer->ClearAttachment(0, &CLEAR_VALUE);
    m_ssao_blur_shader->SetTexture("u_ssao", m_ssao_buffer->GetTexture());
    renderer->Submit(*m_ssao_blur_shader, *m_quad, MeshTopology::TRIANGLES,
                     m_quad->GetIndexBuffer()->GetCount(), 0);
}

void LightingSystem::RenderEmissive() {
    auto lightView = scene->view<TransformComponent, LightComponent>();
    if (lightView.begin() != lightView.end()) {
        device->SetFramebuffer(renderer->GetFramebuffer());
        device->DrawBuffer(renderer->GetFramebuffer(), 0);
        m_emssive_shader->SetTexture("u_lighting",
                                     GetLightingBuffer()->GetTexture());
        m_emssive_shader->SetTexture(
            "u_emissive",
            m_gbuffer->GetTexture(GeometryBufferType::G_EMISSIVE));
        renderer->Submit(*m_emssive_shader, *m_quad, MeshTopology::TRIANGLES,
                         m_quad->GetIndexBuffer()->GetCount(), 0);
    }
}

void LightingSystem::RenderDeferred() {
    auto lightView = scene->view<TransformComponent, LightComponent>();

    // clear the last lighting pass' result
    for (int i = 0; i < 2; ++i) {
        device->SetFramebuffer(m_light_buffer[i].get());
        device->Clear(BufferBitMask::COLOR_BUFFER_BIT);
    }

    m_deferred_shader->SetTexture(
        "u_position", m_gbuffer->GetTexture(GeometryBufferType::G_POSITION));
    m_deferred_shader->SetTexture(
        "u_normal", m_gbuffer->GetTexture(GeometryBufferType::G_NORMAL));
    m_deferred_shader->SetTexture(
        "u_albedo", m_gbuffer->GetTexture(GeometryBufferType::G_ALBEDO));
    m_deferred_shader->SetTexture(
        "u_ambient", m_gbuffer->GetTexture(GeometryBufferType::G_AMBIENT));
    m_deferred_shader->SetTexture("u_background",
                                  renderer->GetFramebuffer()->GetTexture());
    m_deferred_shader->SetTexture("u_ssao", m_ssao_blur_buffer->GetTexture());
    m_deferred_shader->SetBool("u_ssao_state", m_ssao_state);
    const uint8_t input_id = 0;
    const uint8_t output_id = 1;
    lightView.each([this](const TransformComponent &transformComp,
                          const LightComponent &lightComp) {
        device->SetFramebuffer(m_light_buffer[output_id].get());
        renderer->Begin(*m_deferred_shader, *scene->GetCamera());
        const Light &light = lightComp.light;
        m_deferred_shader->SetTexture("u_lighting",
                                      m_light_buffer[input_id]->GetTexture());
        const Transform &transform = transformComp.GetWorldTransform();
        m_deferred_shader->SetVec3("u_light.direction", transform.GetFront());
        m_deferred_shader->SetVec3("u_light.ambient", light.GetAmbient());
        m_deferred_shader->SetVec3("u_light.diffuse", light.GetDiffuse());
        m_deferred_shader->SetVec3("u_light.specular", light.GetSpecular());
        m_deferred_shader->SetVec3("u_light.position", transform.GetPosition());
        m_deferred_shader->SetFloat("u_light.cutoff",
                                    glm::cos(light.GetCutoff()));
        m_deferred_shader->SetFloat("u_light.outer_cutoff",
                                    glm::cos(light.GetOuterCutoff()));
        m_deferred_shader->SetFloat("u_light.constant", light.GetConstant());
        m_deferred_shader->SetFloat("u_light.linear", light.GetLinear());
        m_deferred_shader->SetFloat("u_light.quadratic", light.GetQuadratic());

        m_deferred_shader->SetBool("u_light.is_directional",
                                   light.IsDirectional());
        m_deferred_shader->SetBool("u_light.is_cast_shadow",
                                   light.IsCastShadow());
        if (light.IsCastShadow()) {
            m_deferred_shader->SetTexture("u_light.shadow_map",
                                          light.GetShadowMap()->GetTexture());
        } else {
            m_deferred_shader->SetTexture("u_light.shadow_map", nullptr);
        }
        m_deferred_shader->SetMat4("u_light.projection_view",
                                   light.GetProjectionView());
        renderer->Submit(*m_deferred_shader, *m_quad, MeshTopology::TRIANGLES,
                         m_quad->GetIndexBuffer()->GetCount(), 0);
        std::swap(m_light_buffer[input_id], m_light_buffer[output_id]);
        renderer->End();
    });
}

void LightingSystem::RenderGBuffer() {
    device->SetFramebuffer(m_gbuffer.get());
    device->Clear(BufferBitMask::COLOR_BUFFER_BIT |
                  BufferBitMask::DEPTH_BUFFER_BIT);
    uint32_t id = static_cast<uint32_t>(entt::null);
    m_gbuffer->ClearAttachment(GeometryBufferType::G_ENTITY_ID, &id);

    auto modelView = scene->view<TransformComponent, ModelComponent>();

    renderer->Begin(*m_gbuffer_shader, *scene->GetCamera());

    modelView.each([this](const entt::entity &entity,
                          const TransformComponent &transformComp,
                          const ModelComponent &modelComp) {
        m_gbuffer_shader->SetMat4(
            "u_model", transformComp.GetWorldTransform().GetMatrix());
        m_gbuffer_shader->SetUint("u_entity_id", static_cast<uint32_t>(entity));
        m_gbuffer_shader->SetVec3("u_color", modelComp.color);
        auto model = asset->Get<Model>(modelComp.id);
        if (model) {
            for (const auto &mesh : model->GetMeshes()) {
                auto &material = model->GetMaterials()[mesh.GetMaterialIndex()];
                m_gbuffer_shader->SetTexture(
                    "u_material.diffuse",
                    material.GetTexture(MaterialType::DIFFUSE));
                m_gbuffer_shader->SetTexture(
                    "u_material.specular",
                    material.GetTexture(MaterialType::SPECULAR));
                m_gbuffer_shader->SetTexture(
                    "u_material.ambient",
                    material.GetTexture(MaterialType::AMBIENT));
                m_gbuffer_shader->SetTexture(
                    "u_material.emissive",
                    material.GetTexture(MaterialType::EMISSIVE));
                renderer->DrawMesh(*m_gbuffer_shader, mesh);
            }
        }
    });
    renderer->End();
}

void LightingSystem::SetSSAORadius(float radius) { m_ssao_radius = radius; }

void LightingSystem::SetSSAOBias(float bias) { m_ssao_bias = bias; }

}  // namespace SD
