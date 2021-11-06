#include "Renderer/RenderSystem.hpp"
#include "Renderer/RenderEngine.hpp"
#include "Renderer/Renderer.hpp"
#include "Graphics//Device.hpp"
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

RenderSystem::RenderSystem(int width, int height, int samples)
    : m_blurResult(nullptr) {
    initShaders();
    initQuad();
    initSkybox();
    initBloom(width, height);
    initLighting(width, height, samples);
}

void RenderSystem::onInit() { registerEvent(this, &RenderSystem::onSizeEvent); }

void RenderSystem::onDestroy() {
    unregisterEvent<SizeEvent>(this);
    unregisterEvent<SceneEvent>(this);
}

void RenderSystem::initShaders() {
    m_mainShader = Asset::manager().load<Shader>("shaders/main.glsl");
    m_emssiveShader = Asset::manager().load<Shader>("shaders/emissive.glsl");
    m_deferredShader = Asset::manager().load<Shader>("shaders/deferred.glsl");
    m_blurShader = Asset::manager().load<Shader>("shaders/blur.glsl");
    m_gBufferShader = Asset::manager().load<Shader>("shaders/gbuffer.glsl");
}

void RenderSystem::initLighting(int width, int height, int samples) {
    for (int i = 0; i < 2; ++i) {
        m_lightTarget[i].addTexture(Texture::create(
            width, height, samples, TextureType::TEX_2D_MULTISAMPLE,
            TextureFormat::RGBA, TextureFormatType::FLOAT, TextureWrap::EDGE,
            TextureFilter::LINEAR, TextureMipmapFilter::LINEAR));
        m_lightTarget[i].addTexture(Texture::create(
            width, height, samples, TextureType::TEX_2D_MULTISAMPLE,
            TextureFormat::DEPTH, TextureFormatType::FLOAT, TextureWrap::EDGE,
            TextureFilter::LINEAR, TextureMipmapFilter::LINEAR));
        m_lightTarget[i].createFramebuffer();
    }
}

void RenderSystem::initBloom(int width, int height) {
    for (int i = 0; i < 2; ++i) {
        m_blurTarget[i].addTexture(Texture::create(
            width, height, 1, TextureType::TEX_2D, TextureFormat::RGBA,
            TextureFormatType::FLOAT, TextureWrap::BORDER,
            TextureFilter::LINEAR, TextureMipmapFilter::LINEAR));
        m_blurTarget[i].createFramebuffer();
    }
}

void RenderSystem::initQuad() {
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

void RenderSystem::initSkybox() {
    const float skyboxVertices[] = {
        // front
        -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 1.0,
        // back
        -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, -1.0};

    const uint32_t skyboxIndices[] = {// front
                                      0, 1, 2, 2, 3, 0,
                                      // right
                                      1, 5, 6, 6, 2, 1,
                                      // back
                                      7, 6, 5, 5, 4, 7,
                                      // left
                                      4, 0, 3, 3, 7, 4,
                                      // bottom
                                      4, 5, 1, 1, 0, 4,
                                      // top
                                      3, 2, 6, 6, 7, 3};
    m_skybox = VertexArray::create();
    VertexBufferLayout layout;
    layout.push(BufferDataType::FLOAT, 3);
    auto vbo = VertexBuffer::create(skyboxVertices, sizeof(skyboxVertices),
                                    BufferIOType::STATIC);
    auto ibo = IndexBuffer::create(skyboxIndices, 36, BufferIOType::STATIC);
    m_skybox->addVertexBuffer(vbo, layout);
    m_skybox->setIndexBuffer(ibo);

    m_skyboxShader = Asset::manager().load<Shader>("shaders/skybox.glsl");
}

void RenderSystem::onSizeEvent(const SizeEvent &event) {
    for (int i = 0; i < 2; ++i) {
        m_blurTarget[i].resize(event.width, event.height);
        m_lightTarget[i].resize(event.width, event.height);
    }
}

void RenderSystem::onTick(float) {}

void RenderSystem::onRender() {
    SD_CORE_ASSERT(RenderEngine::getScene(), "No active scene.");
    SD_CORE_ASSERT(RenderEngine::getCamera(), "No camera is set!");

    clear();
    renderGBuffer();
    renderDeferred();
    renderEmissive();
    renderSkybox();
    renderText();
    if (RenderEngine::getBloom()) {
        renderBlur();
    }
    renderMain();
}

void RenderSystem::clear() {
    Device::instance().resetShaderState();
    // clear the last lighting pass' result
    for (int i = 0; i < 2; ++i) {
        Device::instance().setFramebuffer(m_lightTarget[i].getFramebuffer());
        Device::instance().clear();
    }

    Device::instance().setFramebuffer(
        RenderEngine::getGBufferTarget().getFramebuffer());
    Device::instance().clear(BufferBitMask::COLOR_BUFFER_BIT |
                             BufferBitMask::DEPTH_BUFFER_BIT);
    uint32_t id = static_cast<uint32_t>(Entity::INVALID_ID);
    RenderEngine::getGBufferTarget().getFramebuffer()->clearAttachment(
        GeometryBufferType::G_ENTITY_ID, &id);

    Device::instance().setFramebuffer(
        RenderEngine::getRenderTarget().getFramebuffer());
    Device::instance().clear();
}

void RenderSystem::renderSkybox() {
    glm::vec3 pos = RenderEngine::getCamera()->getWorldPosition();
    m_skyboxShader->setMat4("u_model", glm::translate(glm::mat4(1.0f), pos));

    m_skyboxShader->bind();
    Device::instance().setDepthfunc(DepthFunc::LESS_EQUAL);
    Device::instance().setCullFace(Face::FRONT);

    Device::instance().setRenderTarget(getLightingTarget());
    Device::instance().submit(*m_skybox, MeshTopology::TRIANGLES,
                              m_skybox->getIndexBuffer()->getCount(), 0);
    Device::instance().setCullFace(Face::BACK);
    Device::instance().setDepthfunc(DepthFunc::LESS);
}

void RenderSystem::renderMain() {
    Device::instance().setRenderTarget(RenderEngine::getRenderTarget());

    m_mainShader->setBool("u_bloom", RenderEngine::getBloom());
    if (RenderEngine::getBloom()) {
        m_mainShader->setFloat("u_bloomFactor", RenderEngine::getBloomFactor());
        m_mainShader->setTexture("u_blur", m_blurResult);
    }
    m_mainShader->setTexture("u_lighting", getLightingTarget().getTexture());
    m_mainShader->setFloat("u_exposure", RenderEngine::getExposure());
    m_mainShader->bind();
    Device::instance().submit(*m_quad, MeshTopology::TRIANGLES,
                              m_quad->getIndexBuffer()->getCount(), 0);

    Device::instance().blitFramebuffer(
        getLightingTarget().getFramebuffer(), 0,
        RenderEngine::getRenderTarget().getFramebuffer(), 0,
        BufferBitMask::DEPTH_BUFFER_BIT, TextureFilter::NEAREST);
}

void RenderSystem::renderBlur() {
    const int amount = 10;
    bool horizontal = true;
    const int inputId = 0;
    const int outputId = 1;
    Device::instance().blitFramebuffer(getLightingTarget().getFramebuffer(), 0,
                                       m_blurTarget[inputId].getFramebuffer(),
                                       0, BufferBitMask::COLOR_BUFFER_BIT,
                                       TextureFilter::LINEAR);
    m_blurShader->bind();
    for (int i = 0; i < amount; ++i) {
        Device::instance().setRenderTarget(m_blurTarget[outputId]);
        m_blurResult = m_blurTarget[outputId].getTexture();
        m_blurShader->setBool("u_horizontal", horizontal);
        m_blurShader->setTexture("u_image", m_blurTarget[inputId].getTexture());
        Device::instance().submit(*m_quad, MeshTopology::TRIANGLES,
                                  m_quad->getIndexBuffer()->getCount(), 0);
        horizontal = !horizontal;
        std::swap(m_blurTarget[0], m_blurTarget[1]);
    }
}

void RenderSystem::renderText() {
    auto scene = RenderEngine::getScene();
    auto textView = scene->view<TransformComponent, TextComponent>();

    Device::instance().setRenderTarget(getLightingTarget());
    Renderer::beginScene(*RenderEngine::getCamera());
    textView.each([](const TransformComponent &transformComp,
                     const TextComponent &textComp) {
        if (textComp.fontPath.size()) {
            auto font = Asset::manager().load<Font>(textComp.fontPath);
            Renderer::drawText(*font, textComp.text, textComp.pixelSize,
                               transformComp.transform.getWorldTransform(),
                               textComp.color);
        }
    });
    Renderer::endScene();
}

void RenderSystem::renderEmissive() {
    Device::instance().setRenderTarget(getLightingTarget());
    m_emssiveShader->bind();
    m_emssiveShader->setTexture("u_lighting", getLightingTarget().getTexture());
    m_emssiveShader->setTexture("u_gEmissive",
                                RenderEngine::getGBufferTarget().getTexture(
                                    GeometryBufferType::G_EMISSIVE));
    Device::instance().setDepthMask(false);
    Device::instance().submit(*m_quad, MeshTopology::TRIANGLES,
                              m_quad->getIndexBuffer()->getCount(), 0);
    Device::instance().setDepthMask(true);
}

void RenderSystem::renderDeferred() {
    auto scene = RenderEngine::getScene();
    auto lightView = scene->view<TransformComponent, LightComponent>();

    m_deferredShader->bind();
    RenderEngine::updateShader(*m_deferredShader, *RenderEngine::getCamera());
    m_deferredShader->setTexture("u_gPosition",
                                 RenderEngine::getGBufferTarget().getTexture(
                                     GeometryBufferType::G_POSITION));
    m_deferredShader->setTexture("u_gNormal",
                                 RenderEngine::getGBufferTarget().getTexture(
                                     GeometryBufferType::G_NORMAL));
    m_deferredShader->setTexture("u_gAlbedo",
                                 RenderEngine::getGBufferTarget().getTexture(
                                     GeometryBufferType::G_ALBEDO));
    m_deferredShader->setTexture("u_gAmbient",
                                 RenderEngine::getGBufferTarget().getTexture(
                                     GeometryBufferType::G_AMBIENT));
    const uint8_t inputId = 0;
    const uint8_t outputId = 1;
    Device::instance().setDepthMask(false);
    lightView.each([this](const TransformComponent &transformComp,
                          const LightComponent &lightComp) {
        Device::instance().setRenderTarget(m_lightTarget[outputId]);
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
        Device::instance().submit(*m_quad, MeshTopology::TRIANGLES,
                                  m_quad->getIndexBuffer()->getCount(), 0);
        std::swap(m_lightTarget[inputId], m_lightTarget[outputId]);
    });
    Device::instance().setDepthMask(true);
    Device::instance().blitFramebuffer(
        RenderEngine::getGBufferTarget().getFramebuffer(), 0,
        getLightingTarget().getFramebuffer(), 0,
        BufferBitMask::DEPTH_BUFFER_BIT, TextureFilter::NEAREST);
}

void RenderSystem::renderGBuffer() {
    auto scene = RenderEngine::getScene();
    auto terrainView = scene->view<TransformComponent, TerrainComponent>();
    auto modelView = scene->view<TransformComponent, ModelComponent>();

    Device::instance().setRenderTarget(RenderEngine::getGBufferTarget());
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
