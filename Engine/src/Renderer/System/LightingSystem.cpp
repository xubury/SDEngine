#include "Renderer/System/LightingSystem.hpp"
#include "Renderer/Renderer.hpp"
#include "Asset/Asset.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Component.hpp"

namespace SD {

TextureFormat getTextureFormat(GeometryBufferType type) {
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

TextureFormatType getTextureFormatType(GeometryBufferType type) {
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
    : System("Lighting"), m_target(target) {
    initShaders();
    initLighting(width, height, samples);

    const float quadVertices[] = {
        -1.0f, -1.0f, 0.f, 0.f,  0.f,   // bottom left
        1.0f,  -1.0f, 0.f, 1.0f, 0.f,   // bottom right
        1.0f,  1.0f,  0.f, 1.0f, 1.0f,  // top right
        -1.0f, 1.0f,  0.f, 0.f,  1.0f,  // top left
    };
    const uint32_t indices[] = {0, 1, 2, 2, 3, 0};
    auto buffer = VertexBuffer::create(quadVertices, sizeof(quadVertices),
                                       BufferIOType::STATIC);
    auto indexBuffer = IndexBuffer::create(indices, 6, BufferIOType::STATIC);
    m_quad = VertexArray::create();
    VertexBufferLayout layout;
    layout.push(BufferDataType::FLOAT, 3);
    layout.push(BufferDataType::FLOAT, 2);
    m_quad->addVertexBuffer(buffer, layout);
    m_quad->setIndexBuffer(indexBuffer);
}

void LightingSystem::onInit() {
    registerEvent(this, &LightingSystem::onSizeEvent);
}

void LightingSystem::onDestroy() { unregisterEvent<SizeEvent>(this); }

void LightingSystem::initShaders() {
    m_emssiveShader = ShaderLibrary::instance().load("shaders/emissive.glsl");
    m_deferredShader = ShaderLibrary::instance().load("shaders/deferred.glsl");
    m_gBufferShader = ShaderLibrary::instance().load("shaders/gbuffer.glsl");
}

void LightingSystem::initLighting(int width, int height, int samples) {
    for (int i = 0; i < 2; ++i) {
        m_lightTarget[i].addTexture(Texture::create(
            width, height, samples, TextureType::TEX_2D_MULTISAMPLE,
            TextureFormat::RGBA, TextureFormatType::FLOAT, TextureWrap::EDGE,
            TextureFilter::LINEAR, TextureMipmapFilter::LINEAR));
        m_lightTarget[i].createFramebuffer();
    }

    for (int i = 0; i < GeometryBufferType::GBUFFER_COUNT; ++i) {
        m_gBufferTarget.addTexture(Texture::create(
            width, height, samples, TextureType::TEX_2D_MULTISAMPLE,
            getTextureFormat(GeometryBufferType(i)),
            getTextureFormatType(GeometryBufferType(i)), TextureWrap::EDGE,
            TextureFilter::LINEAR, TextureMipmapFilter::LINEAR));
    }
    m_gBufferTarget.createFramebuffer();
}

void LightingSystem::onSizeEvent(const SizeEvent &event) {
    for (int i = 0; i < 2; ++i) {
        m_lightTarget[i].resize(event.width, event.height);
    }
    m_gBufferTarget.resize(event.width, event.height);
}

void LightingSystem::onRender() {
    SD_CORE_ASSERT(renderer->getScene(), "No active scene.");
    SD_CORE_ASSERT(renderer->getCamera(), "No camera is set!");

    clear();
    renderGBuffer();
    renderer->device().setDepthMask(false);
    renderDeferred();
    renderEmissive();
    renderer->device().setDepthMask(true);

    renderer->device().blitFramebuffer(
        m_gBufferTarget.getFramebuffer(), 0, m_target->getFramebuffer(), 0,
        BufferBitMask::DEPTH_BUFFER_BIT, TextureFilter::NEAREST);
}

void LightingSystem::clear() {
    renderer->device().resetShaderState();
    // clear the last lighting pass' result
    for (int i = 0; i < 2; ++i) {
        renderer->device().setFramebuffer(m_lightTarget[i].getFramebuffer());
        renderer->device().clear(BufferBitMask::COLOR_BUFFER_BIT);
    }

    renderer->device().setFramebuffer(m_gBufferTarget.getFramebuffer());
    renderer->device().clear(BufferBitMask::COLOR_BUFFER_BIT |
                             BufferBitMask::DEPTH_BUFFER_BIT);
    uint32_t id = static_cast<uint32_t>(Entity::INVALID_ID);
    m_gBufferTarget.getFramebuffer()->clearAttachment(
        GeometryBufferType::G_ENTITY_ID, &id);
}

void LightingSystem::renderEmissive() {
    renderer->setRenderTarget(*m_target);
    m_emssiveShader->bind();
    m_emssiveShader->setTexture("u_lighting", getLightingTarget().getTexture());
    m_emssiveShader->setTexture(
        "u_gEmissive",
        m_gBufferTarget.getTexture(GeometryBufferType::G_EMISSIVE));
    renderer->submit(*m_quad, MeshTopology::TRIANGLES,
                     m_quad->getIndexBuffer()->getCount(), 0);
}

void LightingSystem::renderDeferred() {
    auto scene = renderer->getScene();
    auto lightView = scene->view<TransformComponent, LightComponent>();

    m_deferredShader->bind();
    renderer->updateShader(*m_deferredShader, *renderer->getCamera());
    m_deferredShader->setTexture(
        "u_gPosition",
        m_gBufferTarget.getTexture(GeometryBufferType::G_POSITION));
    m_deferredShader->setTexture(
        "u_gNormal", m_gBufferTarget.getTexture(GeometryBufferType::G_NORMAL));
    m_deferredShader->setTexture(
        "u_gAlbedo", m_gBufferTarget.getTexture(GeometryBufferType::G_ALBEDO));
    m_deferredShader->setTexture(
        "u_gAmbient",
        m_gBufferTarget.getTexture(GeometryBufferType::G_AMBIENT));
    const uint8_t inputId = 0;
    const uint8_t outputId = 1;
    lightView.each([this](const TransformComponent &transformComp,
                          const LightComponent &lightComp) {
        renderer->setRenderTarget(m_lightTarget[outputId]);
        const Light &light = lightComp.light;
        m_deferredShader->setTexture("u_lighting",
                                     m_lightTarget[inputId].getTexture());
        const Transform &transform = transformComp.transform;
        m_deferredShader->setVec3("u_light.direction",
                                  transform.getWorldFront());
        m_deferredShader->setVec3("u_light.ambient", light.getAmbient());
        m_deferredShader->setVec3("u_light.diffuse", light.getDiffuse());
        m_deferredShader->setVec3("u_light.specular", light.getSpecular());
        m_deferredShader->setVec3("u_light.position",
                                  transform.getWorldPosition());
        m_deferredShader->setFloat("u_light.cutOff",
                                   glm::cos(light.getCutOff()));
        m_deferredShader->setFloat("u_light.outerCutOff",
                                   glm::cos(light.getOuterCutOff()));
        m_deferredShader->setFloat("u_light.constant", light.getConstant());
        m_deferredShader->setFloat("u_light.linear", light.getLinear());
        m_deferredShader->setFloat("u_light.quadratic", light.getQuadratic());

        m_deferredShader->setBool("u_light.isDirectional",
                                  light.isDirectional());
        m_deferredShader->setBool("u_light.isCastShadow", light.isCastShadow());
        m_deferredShader->setTexture("u_light.shadowMap", light.getShadowMap());
        m_deferredShader->setMat4("u_light.projectionView",
                                  light.getProjectionView());
        renderer->submit(*m_quad, MeshTopology::TRIANGLES,
                         m_quad->getIndexBuffer()->getCount(), 0);
        std::swap(m_lightTarget[inputId], m_lightTarget[outputId]);
    });
}

void LightingSystem::renderGBuffer() {
    auto scene = renderer->getScene();
    auto terrainView = scene->view<TransformComponent, TerrainComponent>();
    auto modelView = scene->view<TransformComponent, ModelComponent>();

    renderer->setRenderTarget(m_gBufferTarget);
    renderer->device().disable(Operation::BLEND);

    renderer->updateShader(*m_gBufferShader, *renderer->getCamera());

    m_gBufferShader->bind();
    terrainView.each([this](const entt::entity &entity,
                            const TransformComponent &transformComp,
                            const TerrainComponent &terrainComp) {
        m_gBufferShader->setMat4("u_model",
                                 transformComp.transform.getWorldTransform());
        m_gBufferShader->setUint("u_entityId", static_cast<uint32_t>(entity));
        auto &terrain = terrainComp.terrain;
        auto &material = terrain.getMaterial();
        m_gBufferShader->setTexture("u_material.diffuse",
                                    material.getTexture(MaterialType::DIFFUSE));
        m_gBufferShader->setTexture(
            "u_material.specular", material.getTexture(MaterialType::SPECULAR));
        m_gBufferShader->setTexture("u_material.ambient",
                                    material.getTexture(MaterialType::AMBIENT));
        m_gBufferShader->setTexture(
            "u_material.emissive", material.getTexture(MaterialType::EMISSIVE));
        renderer->drawMesh(terrain.getMesh());
    });

    modelView.each([this](const entt::entity &entity,
                          const TransformComponent &transformComp,
                          const ModelComponent &modelComp) {
        m_gBufferShader->setMat4("u_model",
                                 transformComp.transform.getWorldTransform());
        m_gBufferShader->setUint("u_entityId", static_cast<uint32_t>(entity));
        m_gBufferShader->setVec3("u_color", modelComp.color);
        auto model = AssetManager::instance().get<Model>(modelComp.id);
        if (model) {
            for (const auto &mesh : model->getMeshes()) {
                auto &material = model->getMaterials()[mesh.getMaterialIndex()];
                m_gBufferShader->setTexture(
                    "u_material.diffuse",
                    material.getTexture(MaterialType::DIFFUSE));
                m_gBufferShader->setTexture(
                    "u_material.specular",
                    material.getTexture(MaterialType::SPECULAR));
                m_gBufferShader->setTexture(
                    "u_material.ambient",
                    material.getTexture(MaterialType::AMBIENT));
                m_gBufferShader->setTexture(
                    "u_material.emissive",
                    material.getTexture(MaterialType::EMISSIVE));
                renderer->drawMesh(mesh);
            }
        }
    });
    renderer->device().enable(Operation::BLEND);
}

}  // namespace SD
