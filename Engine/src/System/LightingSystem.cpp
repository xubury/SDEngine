#include "System/LightingSystem.hpp"
#include "Renderer/Renderer.hpp"
#include "Asset/Asset.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Component.hpp"

namespace SD {

TextureFormat GetTextureFormat(GeometryBufferType type) {
    switch (type) {
        case GeometryBufferType::G_ALBEDO:
            return TextureFormat::RGBA;
        case GeometryBufferType::G_POSITION:
        case GeometryBufferType::G_NORMAL:
        case GeometryBufferType::G_AMBIENT:
        case GeometryBufferType::G_EMISSIVE:
            return TextureFormat::RGB;
        case GeometryBufferType::G_ENTITY_ID:
            return TextureFormat::RED;
        case GeometryBufferType::G_DEPTH:
            return TextureFormat::DEPTH;
        default:
            SD_CORE_WARN("Unknown GBuffer!");
            return TextureFormat::RGBA;
    }
}

TextureFormatType GetTextureFormatType(GeometryBufferType type) {
    switch (type) {
        case GeometryBufferType::G_POSITION:
        case GeometryBufferType::G_NORMAL:
        case GeometryBufferType::G_DEPTH:
            return TextureFormatType::FLOAT;
        case GeometryBufferType::G_ALBEDO:
        case GeometryBufferType::G_AMBIENT:
        case GeometryBufferType::G_EMISSIVE:
            return TextureFormatType::UBYTE;
        case GeometryBufferType::G_ENTITY_ID:
            return TextureFormatType::UINT;
        default:
            SD_CORE_WARN("Unknown GBuffer!");
            return TextureFormatType::UBYTE;
    }
}

LightingSystem::LightingSystem(RenderTarget *target, int width, int height,
                               int samples)
    : System("LightingSystem"), m_target(target) {
    InitLighting(width, height, samples);

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
    layout.Push(BufferDataType::FLOAT3);
    layout.Push(BufferDataType::FLOAT2);
    m_quad->AddVertexBuffer(buffer, layout);
    m_quad->SetIndexBuffer(indexBuffer);
}

void LightingSystem::OnInit() { InitShaders(); }

void LightingSystem::OnPush() {
    m_size_handler = dispatcher->Register(this, &LightingSystem::OnSizeEvent);
}

void LightingSystem::OnPop() { dispatcher->RemoveHandler(m_size_handler); }

void LightingSystem::InitShaders() {
    m_shadow_shader = ShaderLibrary::Instance().Load("shaders/shadow.glsl");
    m_emssive_shader = ShaderLibrary::Instance().Load("shaders/emissive.glsl");
    m_deferred_shader = ShaderLibrary::Instance().Load("shaders/deferred.glsl");
    m_gbuffer_shader = ShaderLibrary::Instance().Load("shaders/gbuffer.glsl");
}

void LightingSystem::InitLighting(int width, int height, int samples) {
    for (int i = 0; i < 2; ++i) {
        m_light_target[i].AddTexture(Texture::Create(
            width, height, samples, TextureType::TEX_2D_MULTISAMPLE,
            TextureFormat::RGBA, TextureFormatType::FLOAT, TextureWrap::EDGE,
            TextureFilter::LINEAR, TextureMipmapFilter::LINEAR));
        m_light_target[i].CreateFramebuffer();
    }

    for (int i = 0; i < GeometryBufferType::GBUFFER_COUNT; ++i) {
        m_gbuffer_target.AddTexture(Texture::Create(
            width, height, samples, TextureType::TEX_2D_MULTISAMPLE,
            GetTextureFormat(GeometryBufferType(i)),
            GetTextureFormatType(GeometryBufferType(i)), TextureWrap::EDGE,
            TextureFilter::LINEAR, TextureMipmapFilter::LINEAR));
    }
    m_gbuffer_target.CreateFramebuffer();
}

void LightingSystem::OnSizeEvent(const WindowSizeEvent &event) {
    for (int i = 0; i < 2; ++i) {
        m_light_target[i].Resize(event.width, event.height);
    }
    m_gbuffer_target.Resize(event.width, event.height);
}

void LightingSystem::OnRender() {
    SD_CORE_ASSERT(renderer->GetScene(), "No active scene.");
    SD_CORE_ASSERT(renderer->GetCamera(), "No camera is set!");

    Clear();
    RenderShadowMap();
    Device::instance().Disable(Operation::BLEND);
    RenderGBuffer();
    Device::instance().Enable(Operation::BLEND);
    Device::instance().SetDepthMask(false);
    RenderDeferred();
    RenderEmissive();
    Device::instance().SetDepthMask(true);

    Device::instance().BlitFramebuffer(
        m_gbuffer_target.GetFramebuffer(), 0, m_target->GetFramebuffer(), 0,
        BufferBitMask::DEPTH_BUFFER_BIT, TextureFilter::NEAREST);
}

void LightingSystem::Clear() {
    Device::instance().ResetShaderState();
    // clear the last lighting pass' result
    for (int i = 0; i < 2; ++i) {
        Device::instance().SetFramebuffer(m_light_target[i].GetFramebuffer());
        Device::instance().Clear(BufferBitMask::COLOR_BUFFER_BIT);
    }

    Device::instance().SetFramebuffer(m_gbuffer_target.GetFramebuffer());
    Device::instance().Clear(BufferBitMask::COLOR_BUFFER_BIT |
                             BufferBitMask::DEPTH_BUFFER_BIT);
    uint32_t id = static_cast<uint32_t>(Entity::INVALID_ID);
    m_gbuffer_target.GetFramebuffer()->ClearAttachment(
        GeometryBufferType::G_ENTITY_ID, &id);
}

void LightingSystem::RenderShadowMap() {
    auto scene = renderer->GetScene();
    auto lightView = scene->view<TransformComponent, LightComponent>();
    auto modelView = scene->view<TransformComponent, ModelComponent>();
    Device::instance().SetCullFace(Face::FRONT);
    m_shadow_shader->Bind();
    lightView.each([this, &modelView](const TransformComponent &transformComp,
                                      LightComponent &lightComp) {
        Light &light = lightComp.light;
        if (!light.IsCastShadow()) return;

        renderer->SetRenderTarget(light.GetRenderTarget());
        light.GetRenderTarget().GetFramebuffer()->ClearDepth();
        light.ComputeLightSpaceMatrix(transformComp.transform,
                                      renderer->GetCamera());
        m_shadow_shader->SetMat4("u_projectionView", light.GetProjectionView());

        modelView.each([this](const TransformComponent &transformComp,
                              const ModelComponent &modelComp) {
            auto model = asset->Get<Model>(modelComp.id);
            m_shadow_shader->SetMat4(
                "u_model", transformComp.transform.GetWorldTransform());
            if (model) {
                for (const auto &mesh : model->GetMeshes()) {
                    renderer->DrawMesh(mesh);
                }
            }
        });
    });
    Device::instance().SetCullFace(Face::BACK);
}

void LightingSystem::RenderEmissive() {
    renderer->SetRenderTarget(*m_target);
    m_emssive_shader->Bind();
    m_emssive_shader->SetTexture("u_lighting",
                                 GetLightingTarget().GetTexture());
    m_emssive_shader->SetTexture(
        "u_emissive",
        m_gbuffer_target.GetTexture(GeometryBufferType::G_EMISSIVE));
    renderer->Submit(*m_quad, MeshTopology::TRIANGLES,
                     m_quad->GetIndexBuffer()->GetCount(), 0);
}

void LightingSystem::RenderDeferred() {
    auto scene = renderer->GetScene();
    auto lightView = scene->view<TransformComponent, LightComponent>();

    m_deferred_shader->Bind();
    renderer->UpdateShader(*m_deferred_shader, *renderer->GetCamera());
    m_deferred_shader->SetTexture(
        "u_position",
        m_gbuffer_target.GetTexture(GeometryBufferType::G_POSITION));
    m_deferred_shader->SetTexture(
        "u_normal", m_gbuffer_target.GetTexture(GeometryBufferType::G_NORMAL));
    m_deferred_shader->SetTexture(
        "u_albedo", m_gbuffer_target.GetTexture(GeometryBufferType::G_ALBEDO));
    m_deferred_shader->SetTexture(
        "u_ambient",
        m_gbuffer_target.GetTexture(GeometryBufferType::G_AMBIENT));
    const uint8_t inputId = 0;
    const uint8_t outputId = 1;
    lightView.each([this](const TransformComponent &transformComp,
                          const LightComponent &lightComp) {
        renderer->SetRenderTarget(m_light_target[outputId]);
        const Light &light = lightComp.light;
        m_deferred_shader->SetTexture("u_lighting",
                                      m_light_target[inputId].GetTexture());
        const Transform &transform = transformComp.transform;
        m_deferred_shader->SetVec3("u_light.direction",
                                   transform.GetWorldFront());
        m_deferred_shader->SetVec3("u_light.ambient", light.GetAmbient());
        m_deferred_shader->SetVec3("u_light.diffuse", light.GetDiffuse());
        m_deferred_shader->SetVec3("u_light.specular", light.GetSpecular());
        m_deferred_shader->SetVec3("u_light.position",
                                   transform.GetWorldPosition());
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
        m_deferred_shader->SetTexture("u_light.shadow_map",
                                      light.GetShadowMap());
        m_deferred_shader->SetMat4("u_light.projection_view",
                                   light.GetProjectionView());
        renderer->Submit(*m_quad, MeshTopology::TRIANGLES,
                         m_quad->GetIndexBuffer()->GetCount(), 0);
        std::swap(m_light_target[inputId], m_light_target[outputId]);
    });
}

void LightingSystem::RenderGBuffer() {
    auto scene = renderer->GetScene();
    auto terrainView = scene->view<TransformComponent, TerrainComponent>();
    auto modelView = scene->view<TransformComponent, ModelComponent>();

    renderer->SetRenderTarget(m_gbuffer_target);

    renderer->UpdateShader(*m_gbuffer_shader, *renderer->GetCamera());

    m_gbuffer_shader->Bind();
    terrainView.each([this](const entt::entity &entity,
                            const TransformComponent &transformComp,
                            const TerrainComponent &terrainComp) {
        m_gbuffer_shader->SetMat4("u_model",
                                  transformComp.transform.GetWorldTransform());
        m_gbuffer_shader->SetUint("u_entity_id", static_cast<uint32_t>(entity));
        auto &terrain = terrainComp.terrain;
        auto &material = terrain.GetMaterial();
        m_gbuffer_shader->SetTexture(
            "u_material.diffuse", material.GetTexture(MaterialType::DIFFUSE));
        m_gbuffer_shader->SetTexture(
            "u_material.specular", material.GetTexture(MaterialType::SPECULAR));
        m_gbuffer_shader->SetTexture(
            "u_material.ambient", material.GetTexture(MaterialType::AMBIENT));
        m_gbuffer_shader->SetTexture(
            "u_material.emissive", material.GetTexture(MaterialType::EMISSIVE));
        renderer->DrawMesh(terrain.GetMesh());
    });

    modelView.each([this](const entt::entity &entity,
                          const TransformComponent &transformComp,
                          const ModelComponent &modelComp) {
        m_gbuffer_shader->SetMat4("u_model",
                                  transformComp.transform.GetWorldTransform());
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
                renderer->DrawMesh(mesh);
            }
        }
    });
}

}  // namespace SD
