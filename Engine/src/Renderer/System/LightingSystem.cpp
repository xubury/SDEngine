#include "Renderer/System/LightingSystem.hpp"
#include "Renderer/Renderer.hpp"
#include "Utility/Loader/AssetLoader.hpp"
#include "Utility/Log.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Component.hpp"

namespace sd {

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

LightingSystem::LightingSystem(int width, int height, int samples) {
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
    m_emssiveShader = Asset::manager().load<Shader>("shaders/emissive.glsl");
    m_deferredShader = Asset::manager().load<Shader>("shaders/deferred.glsl");
    m_gBufferShader = Asset::manager().load<Shader>("shaders/gbuffer.glsl");
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
    SD_CORE_ASSERT(Renderer::engine().getScene(), "No active scene.");
    SD_CORE_ASSERT(Renderer::engine().getCamera(), "No camera is set!");

    clear();
    renderGBuffer();
    Renderer::device().setDepthMask(false);
    renderDeferred();
    renderEmissive();
    Renderer::device().setDepthMask(true);

    Renderer::device().blitFramebuffer(
        m_gBufferTarget.getFramebuffer(), 0,
        Renderer::engine().getRenderTarget().getFramebuffer(), 0,
        BufferBitMask::DEPTH_BUFFER_BIT, TextureFilter::NEAREST);
}

void LightingSystem::clear() {
    Renderer::device().resetShaderState();
    // clear the last lighting pass' result
    for (int i = 0; i < 2; ++i) {
        Renderer::device().setFramebuffer(m_lightTarget[i].getFramebuffer());
        Renderer::device().clear(BufferBitMask::COLOR_BUFFER_BIT);
    }

    Renderer::device().setFramebuffer(m_gBufferTarget.getFramebuffer());
    Renderer::device().clear(BufferBitMask::COLOR_BUFFER_BIT |
                             BufferBitMask::DEPTH_BUFFER_BIT);
    uint32_t id = static_cast<uint32_t>(Entity::INVALID_ID);
    m_gBufferTarget.getFramebuffer()->clearAttachment(
        GeometryBufferType::G_ENTITY_ID, &id);
}

void LightingSystem::renderEmissive() {
    Renderer::engine().getRenderTarget().bind();
    m_emssiveShader->bind();
    m_emssiveShader->setTexture("u_lighting", getLightingTarget().getTexture());
    m_emssiveShader->setTexture(
        "u_gEmissive",
        m_gBufferTarget.getTexture(GeometryBufferType::G_EMISSIVE));
    Renderer::submit(*m_quad, MeshTopology::TRIANGLES,
                     m_quad->getIndexBuffer()->getCount(), 0);
}

void LightingSystem::renderDeferred() {
    auto scene = Renderer::engine().getScene();
    auto lightView = scene->view<TransformComponent, LightComponent>();

    m_deferredShader->bind();
    Renderer::engine().updateShader(*m_deferredShader,
                                    *Renderer::engine().getCamera());
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
        m_lightTarget[outputId].bind();
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
        Renderer::submit(*m_quad, MeshTopology::TRIANGLES,
                         m_quad->getIndexBuffer()->getCount(), 0);
        std::swap(m_lightTarget[inputId], m_lightTarget[outputId]);
    });
}

void LightingSystem::renderGBuffer() {
    auto scene = Renderer::engine().getScene();
    auto terrainView = scene->view<TransformComponent, TerrainComponent>();
    auto modelView = scene->view<TransformComponent, ModelComponent>();

    m_gBufferTarget.bind();
    Renderer::device().disable(Operation::BLEND);

    Renderer::engine().updateShader(*m_gBufferShader,
                                    *Renderer::engine().getCamera());

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
        Renderer3D::drawMesh(terrain.getMesh());
    });

    modelView.each([this](const entt::entity &entity,
                          const TransformComponent &transformComp,
                          const ModelComponent &modelComp) {
        m_gBufferShader->setMat4("u_model",
                                 transformComp.transform.getWorldTransform());
        m_gBufferShader->setUint("u_entityId", static_cast<uint32_t>(entity));
        m_gBufferShader->setVec3("u_color", modelComp.color);
        for (const auto &mesh : modelComp.model->getMeshes()) {
            auto &material =
                modelComp.model->getMaterials()[mesh.getMaterialIndex()];
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
            Renderer3D::drawMesh(mesh);
        }
    });
    Renderer::device().enable(Operation::BLEND);
}

}  // namespace sd
