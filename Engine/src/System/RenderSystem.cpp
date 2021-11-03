#include "System/RenderSystem.hpp"
#include "Core/Application.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Renderer.hpp"
#include "Utility/Log.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Component.hpp"

namespace sd {

inline TextureFormat getTextureFormat(GeometryBufferType type) {
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
        default:
            SD_CORE_WARN("Unknown GBuffer!");
            return TextureFormat::RGBA;
    }
}

inline TextureFormatType getTextureFormatType(GeometryBufferType type) {
    switch (type) {
        case GeometryBufferType::G_POSITION:
        case GeometryBufferType::G_NORMAL:
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
inline Ref<Texture> createTexture(int width, int height, int samples,
                                  TextureFormat format, TextureFormatType type,
                                  bool linear) {
    return Texture::create(
        width, height, samples,
        samples > 1 ? TextureType::TEX_2D_MULTISAMPLE : TextureType::TEX_2D,
        format, type, TextureWrap::BORDER,
        linear ? TextureFilter::LINEAR : TextureFilter::NEAREST,
        linear ? TextureMipmapFilter::LINEAR : TextureMipmapFilter::NEAREST);
}

RenderSystem::RenderSystem(int width, int height, int samples)
    : m_blurResult(nullptr) {
    initShaders();
    initQuad();
    initSkybox();
    initBloom(width, height);
    initLighting(width, height);
    initGBuffer(width, height, samples);
}

void RenderSystem::onInit() { registerEvent(this, &RenderSystem::onSizeEvent); }

void RenderSystem::onDestroy() {
    unregisterEvent<SizeEvent>(this);
    unregisterEvent<SceneEvent>(this);
}

void RenderSystem::initShaders() {
    m_mainShader = Asset::manager().load<Shader>("shaders/main.glsl");
    m_emssiveShader = Asset::manager().load<Shader>("shaders/emissive.glsl");
    m_lightShader = Asset::manager().load<Shader>("shaders/lighting.glsl");
    m_blurShader = Asset::manager().load<Shader>("shaders/blur.glsl");
    m_gBufferShader = Asset::manager().load<Shader>("shaders/gbuffer.glsl");
}

void RenderSystem::initLighting(int width, int height) {
    for (int i = 0; i < 2; ++i) {
        m_lightTarget[i].addTexture(Texture::create(
            width, height, 1, TextureType::TEX_2D, TextureFormat::RGBA,
            TextureFormatType::FLOAT, TextureWrap::BORDER,
            TextureFilter::LINEAR, TextureMipmapFilter::LINEAR));
        m_lightTarget[i].addTexture(Texture::create(
            width, height, 1, TextureType::TEX_2D, TextureFormat::DEPTH,
            TextureFormatType::FLOAT, TextureWrap::BORDER,
            TextureFilter::LINEAR, TextureMipmapFilter::LINEAR));
        m_lightTarget[i].createFramebuffer();
    }
}

void RenderSystem::initBloom(int width, int height) {
    for (int i = 0; i < 2; ++i) {
        m_blurTarget[i].addTexture(Texture::create(
            width, height, 1, TextureType::TEX_2D, TextureFormat::RGBA,
            TextureFormatType::UBYTE, TextureWrap::BORDER,
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
    auto buffer = VertexBuffer::create(quadVertices, 20, sizeof(float),
                                       BufferIOType::STATIC);
    auto indexBuffer =
        IndexBuffer::create(indices, 6, sizeof(uint32_t), BufferIOType::STATIC);
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
    auto vbo = VertexBuffer::create(skyboxVertices, 24, sizeof(float),
                                    BufferIOType::STATIC);
    auto ibo = IndexBuffer::create(skyboxIndices, 36, sizeof(uint32_t),
                                   BufferIOType::STATIC);
    m_skybox->addVertexBuffer(vbo, layout);
    m_skybox->setIndexBuffer(ibo);

    m_skyboxShader = Asset::manager().load<Shader>("shaders/skybox.glsl");
}

void RenderSystem::initGBuffer(int width, int height, int samples) {
    m_gBuffer = Framebuffer::create();
    m_gBuffer->clear();
    m_gBufferTarget.clear();
    for (int i = 0; i < GeometryBufferType::GBUFFER_COUNT; ++i) {
        TextureFormat format = getTextureFormat(GeometryBufferType(i));
        TextureFormatType type = getTextureFormatType(GeometryBufferType(i));
        bool linear = i != GeometryBufferType::G_ENTITY_ID;
        m_gBufferTarget.addTexture(
            createTexture(width, height, samples, format, type, linear));
        m_gBuffer->attachTexture(
            createTexture(width, height, 1, format, type, linear));
    }
    // depth
    m_gBuffer->attachTexture(createTexture(width, height, 1,
                                           TextureFormat::DEPTH,
                                           TextureFormatType::FLOAT, true));
    m_gBufferTarget.addTexture(createTexture(width, height, samples,
                                             TextureFormat::DEPTH,
                                             TextureFormatType::FLOAT, true));
    m_gBufferTarget.createFramebuffer();
}

void RenderSystem::onSizeEvent(const SizeEvent &event) {
    for (int i = 0; i < 2; ++i) {
        m_lightTarget[i].resize(event.width, event.height);
        m_blurTarget[i].resize(event.width, event.height);
    }
    m_gBuffer->resize(event.width, event.height);
    m_gBufferTarget.resize(event.width, event.height);
}

void RenderSystem::onTick(float) {}

void RenderSystem::onRender() {
    SD_CORE_ASSERT(Application::getRenderEngine().getScene(),
                   "No active scene.");
    SD_CORE_ASSERT(Application::getRenderEngine().getCamera(),
                   "No camera is set!");

    clear();
    renderGBuffer();
    renderLight();
    renderEmissive();
    renderSkybox();
    renderText();
    if (Application::getRenderEngine().getBloom()) {
        renderBlur();
    }
    renderMain();
}

void RenderSystem::clear() {
    Device::instance().resetShaderState();
    // clear the last lighting pass' result
    RGBA color(0, 0, 0, 1);
    getLightResult().getFramebuffer()->clearAttachment(0, color.data());

    Device::instance().setFramebuffer(m_gBufferTarget.getFramebuffer());
    Device::instance().clear(BufferBitMask::COLOR_BUFFER_BIT |
                             BufferBitMask::DEPTH_BUFFER_BIT);
    m_gBufferTarget.getFramebuffer()->clearAttachment(
        GeometryBufferType::G_ENTITY_ID, &Entity::INVALID_ID);

    Device::instance().setFramebuffer(
        Application::getRenderEngine().getRenderTarget().getFramebuffer());
    Device::instance().clear();
}

void RenderSystem::renderSkybox() {
    glm::vec3 pos =
        Application::getRenderEngine().getCamera()->getWorldPosition();
    m_skyboxShader->setMat4("u_model", glm::translate(glm::mat4(1.0f), pos));

    m_skyboxShader->bind();
    Device::instance().setDepthfunc(DepthFunc::LESS_EQUAL);
    Device::instance().setCullFace(Face::FRONT);

    Renderer::setRenderTarget(getLightResult());
    Renderer::beginScene(*Application::getRenderEngine().getCamera());
    Renderer::submit(*m_skybox, MeshTopology::TRIANGLES,
                     m_skybox->getIndexBuffer()->getCount(), 0);
    Renderer::endScene();
    Device::instance().setCullFace(Face::BACK);
    Device::instance().setDepthfunc(DepthFunc::LESS);
}

void RenderSystem::renderMain() {
    Renderer::setRenderTarget(Application::getRenderEngine().getRenderTarget());

    m_mainShader->setBool("u_bloom", Application::getRenderEngine().getBloom());
    if (Application::getRenderEngine().getBloom()) {
        m_mainShader->setFloat("u_bloomFactor",
                               Application::getRenderEngine().getBloomFactor());
        m_mainShader->setTexture("u_blur", m_blurResult);
    }
    m_mainShader->setTexture("u_lighting", getLightResult().getTexture());
    m_mainShader->setFloat("u_exposure",
                           Application::getRenderEngine().getExposure());
    m_mainShader->bind();
    Renderer::submit(*m_quad, MeshTopology::TRIANGLES,
                     m_quad->getIndexBuffer()->getCount(), 0);

    Device::instance().blitFramebuffer(
        getLightResult().getFramebuffer(), nullptr, 0,
        BufferBitMask::DEPTH_BUFFER_BIT, TextureFilter::NEAREST);
}

void RenderSystem::renderBlur() {
    const int amount = 10;
    bool horizontal = true;
    m_blurShader->bind();
    for (int i = 0; i < amount; ++i) {
        const int inputId = horizontal;
        const int outputId = !horizontal;
        Renderer::setRenderTarget(m_blurTarget[outputId]);
        m_blurResult = m_blurTarget[outputId].getTexture();
        m_blurShader->setBool("u_horizontal", horizontal);
        m_blurShader->setTexture("u_image",
                                 i == 0 ? getLightResult().getTexture()
                                        : m_blurTarget[inputId].getTexture());
        Renderer::submit(*m_quad, MeshTopology::TRIANGLES,
                         m_quad->getIndexBuffer()->getCount(), 0);
        horizontal = !horizontal;
    }
}

void RenderSystem::renderText() {
    auto scene = Application::getRenderEngine().getScene();
    auto textView = scene->view<TransformComponent, TextComponent>();

    Renderer::setRenderTarget(getLightResult());
    Renderer::beginScene(*Application::getRenderEngine().getCamera());
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
    Renderer::setRenderTarget(getLightResult());
    m_emssiveShader->bind();
    m_emssiveShader->setTexture("u_lighting", getLightResult().getTexture());
    m_emssiveShader->setTexture(
        "u_gEmissive", m_gBuffer->getTexture(GeometryBufferType::G_EMISSIVE));
    Device::instance().setDepthMask(false);
    Renderer::submit(*m_quad, MeshTopology::TRIANGLES,
                     m_quad->getIndexBuffer()->getCount(), 0);
    Device::instance().setDepthMask(true);
}

void RenderSystem::renderLight() {
    auto scene = Application::getRenderEngine().getScene();
    auto lightView = scene->view<TransformComponent, LightComponent>();

    m_lightShader->bind();
    Device::instance().setDepthMask(false);
    Renderer::setShader(*m_lightShader);
    m_lightShader->setTexture(
        "u_gPosition", m_gBuffer->getTexture(GeometryBufferType::G_POSITION));
    m_lightShader->setTexture(
        "u_gNormal", m_gBuffer->getTexture(GeometryBufferType::G_NORMAL));
    m_lightShader->setTexture(
        "u_gAlbedo", m_gBuffer->getTexture(GeometryBufferType::G_ALBEDO));
    m_lightShader->setTexture(
        "u_gAmbient", m_gBuffer->getTexture(GeometryBufferType::G_AMBIENT));
    const uint8_t inputIndex = 0;
    const uint8_t outputIndex = 1;

    lightView.each([this](const TransformComponent &transformComp,
                          const LightComponent &lightComp) {
        Renderer::setRenderTarget(m_lightTarget[outputIndex]);
        const Light &light = lightComp.light;
        m_lightShader->setTexture("u_lighting",
                                  m_lightTarget[inputIndex].getTexture());
        const Transform &transform = transformComp.transform;
        m_lightShader->setVec3("u_light.direction", transform.getWorldFront());
        m_lightShader->setVec3("u_light.ambient", light.getAmbient());
        m_lightShader->setVec3("u_light.diffuse", light.getDiffuse());
        m_lightShader->setVec3("u_light.specular", light.getSpecular());
        m_lightShader->setVec3("u_light.position",
                               transform.getWorldPosition());
        m_lightShader->setFloat("u_light.cutOff", glm::cos(light.getCutOff()));
        m_lightShader->setFloat("u_light.outerCutOff",
                                glm::cos(light.getOuterCutOff()));
        m_lightShader->setFloat("u_light.constant", light.getConstant());
        m_lightShader->setFloat("u_light.linear", light.getLinear());
        m_lightShader->setFloat("u_light.quadratic", light.getQuadratic());

        m_lightShader->setBool("u_light.isDirectional", light.isDirectional());
        m_lightShader->setBool("u_light.isCastShadow", light.isCastShadow());
        m_lightShader->setTexture("u_light.shadowMap", light.getShadowMap());
        m_lightShader->setMat4("u_light.projectionView",
                               light.getProjectionView());
        Renderer::submit(*m_quad, MeshTopology::TRIANGLES,
                         m_quad->getIndexBuffer()->getCount(), 0);
        std::swap(m_lightTarget[outputIndex], m_lightTarget[inputIndex]);
    });
    Device::instance().setDepthMask(true);

    Device::instance().blitFramebuffer(
        m_gBuffer.get(), getLightResult().getFramebuffer(), 0,
        BufferBitMask::DEPTH_BUFFER_BIT, TextureFilter::NEAREST);
}

void RenderSystem::renderGBuffer() {
    auto scene = Application::getRenderEngine().getScene();
    auto terrainView = scene->view<TransformComponent, TerrainComponent>();
    auto modelView = scene->view<TransformComponent, ModelComponent>();

    Renderer::setRenderTarget(m_gBufferTarget);

    m_gBufferShader->bind();
    Device::instance().disable(Operation::BLEND);
    Renderer::beginScene(*Application::getRenderEngine().getCamera());
    Renderer::setShader(*m_gBufferShader);
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

    Device::instance().blitFramebuffer(
        m_gBufferTarget.getFramebuffer(), m_gBuffer.get(), G_ENTITY_ID,
        BufferBitMask::COLOR_BUFFER_BIT | BufferBitMask::DEPTH_BUFFER_BIT,
        TextureFilter::NEAREST);
    for (int i = 0; i < GeometryBufferType::G_ENTITY_ID; ++i) {
        Device::instance().blitFramebuffer(
            m_gBufferTarget.getFramebuffer(), m_gBuffer.get(), i,
            BufferBitMask::COLOR_BUFFER_BIT, TextureFilter::LINEAR);
    }
    Renderer::endScene();
    Device::instance().enable(Operation::BLEND);
}

}  // namespace sd
