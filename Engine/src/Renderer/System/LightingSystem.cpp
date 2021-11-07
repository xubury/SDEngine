#include "Renderer/System/LightingSystem.hpp"
#include "Renderer/RenderEngine.hpp"
#include "Renderer/Renderer.hpp"
#include "Graphics/Device.hpp"
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
    SD_CORE_ASSERT(RenderEngine::getScene(), "No active scene.");
    SD_CORE_ASSERT(RenderEngine::getCamera(), "No camera is set!");

    clear();
    renderGBuffer();
    Device::instance().setDepthMask(false);
    renderDeferred();
    renderEmissive();
    Device::instance().setDepthMask(true);

    Device::instance().blitFramebuffer(
        m_gBufferTarget.getFramebuffer(), 0,
        RenderEngine::getRenderTarget().getFramebuffer(), 0,
        BufferBitMask::DEPTH_BUFFER_BIT, TextureFilter::NEAREST);
}

void LightingSystem::clear() {
    Device::instance().resetShaderState();
    // clear the last lighting pass' result
    for (int i = 0; i < 2; ++i) {
        Device::instance().setFramebuffer(m_lightTarget[i].getFramebuffer());
        Device::instance().clear(BufferBitMask::COLOR_BUFFER_BIT);
    }

    Device::instance().setFramebuffer(m_gBufferTarget.getFramebuffer());
    Device::instance().clear(BufferBitMask::COLOR_BUFFER_BIT |
                             BufferBitMask::DEPTH_BUFFER_BIT);
    uint32_t id = static_cast<uint32_t>(Entity::INVALID_ID);
    m_gBufferTarget.getFramebuffer()->clearAttachment(
        GeometryBufferType::G_ENTITY_ID, &id);
}

void LightingSystem::renderEmissive() {
    RenderEngine::getRenderTarget().bind();
    m_emssiveShader->bind();
    m_emssiveShader->setTexture("u_lighting", getLightingTarget().getTexture());
    m_emssiveShader->setTexture(
        "u_gEmissive",
        m_gBufferTarget.getTexture(GeometryBufferType::G_EMISSIVE));
    RenderEngine::renderQuad();
}

void LightingSystem::renderDeferred() {
    auto scene = RenderEngine::getScene();
    auto lightView = scene->view<TransformComponent, LightComponent>();

    m_deferredShader->bind();
    RenderEngine::updateShader(*m_deferredShader, *RenderEngine::getCamera());
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
        RenderEngine::renderQuad();
        std::swap(m_lightTarget[inputId], m_lightTarget[outputId]);
    });
}

void LightingSystem::renderGBuffer() {
    auto scene = RenderEngine::getScene();
    auto terrainView = scene->view<TransformComponent, TerrainComponent>();
    auto modelView = scene->view<TransformComponent, ModelComponent>();

    m_gBufferTarget.bind();
    Device::instance().disable(Operation::BLEND);

    RenderEngine::updateShader(*m_gBufferShader, *RenderEngine::getCamera());

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
        Renderer::drawMesh(terrain.getMesh());
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
            Renderer::drawMesh(mesh);
        }
    });
    Device::instance().enable(Operation::BLEND);
}

}  // namespace sd
