#include "System/RenderSystem.hpp"
#include "Core/Application.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Renderer.hpp"
#include "Utility/Log.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Component.hpp"

namespace sd {

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

inline TextureFormat getTextureFormat(GeometryBufferType type) {
    switch (type) {
        case GeometryBufferType::G_POSITION:
        case GeometryBufferType::G_NORMAL:
        case GeometryBufferType::G_ALBEDO:
            return TextureFormat::RGBA;
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

RenderSystem::RenderSystem(int width, int height, int samples)
    : m_blurResult(nullptr),
      m_gBuffer(Framebuffer::create()),
      m_scene(nullptr),
      m_camera(nullptr),
      m_exposure(1.5f),
      m_bloom(1.0f),
      m_isBloom(true) {
    if (samples > 1) {
        Device::instance().enable(Operation::MULTISAMPLE);
    }
    m_mainShader = Asset::manager().load<Shader>("shaders/main.glsl");
    m_lightShader = Asset::manager().load<Shader>("shaders/lighting.glsl");
    m_blurShader = Asset::manager().load<Shader>("shaders/blur.glsl");
    m_shadowShader = Asset::manager().load<Shader>("shaders/shadow.glsl");

    for (int i = 0; i < 2; ++i) {
        m_blurTarget[i].addTexture(
            createTexture(width, height, 1, TextureFormat::RGBA,
                          TextureFormatType::UBYTE, true));
        m_lightTarget[i].addTexture(
            createTexture(width, height, 1, TextureFormat::RGBA,
                          TextureFormatType::FLOAT, true));
        m_lightTarget[i].addTexture(
            createTexture(width, height, 1, TextureFormat::DEPTH,
                          TextureFormatType::FLOAT, true));
        m_blurTarget[i].createFramebuffer();
        m_lightTarget[i].createFramebuffer();
    }

    initQuad();
    initGBuffer(width, height, samples);
}

void RenderSystem::onInit() {
    registerEvent(this, &RenderSystem::onSizeEvent);
    registerEvent(this, &RenderSystem::onSceneEvent);
}

void RenderSystem::onDestroy() {
    unregisterEvent<SizeEvent>(this);
    unregisterEvent<SceneEvent>(this);
}

void RenderSystem::initQuad() {
    float quadVertices[] = {
        1.0f,  1.0f,  0.f, 1.0f, 1.0f,  // top right
        1.0f,  -1.0f, 0.f, 1.0f, 0.f,   // bottom right
        -1.0f, -1.0f, 0.f, 0.f,  0.f,   // bottom left
        -1.0f, 1.0f,  0.f, 0.f,  1.0f   // top left
    };
    unsigned int indices[] = {0, 1, 3, 1, 2, 3};
    Ref<VertexBuffer> buffer = VertexBuffer::create(
        quadVertices, sizeof(quadVertices), BufferIOType::STATIC);
    Ref<IndexBuffer> indexBuffer =
        IndexBuffer::create(indices, 6, BufferIOType::STATIC);
    m_quad = VertexArray::create();
    VertexBufferLayout layout;
    layout.push<float>(3);
    layout.push<float>(2);
    m_quad->addVertexBuffer(buffer, layout);
    m_quad->setIndexBuffer(indexBuffer);
}

void RenderSystem::initGBuffer(int width, int height, int samples) {
    m_gBufferShader = Asset::manager().load<Shader>("shaders/gbuffer.glsl");
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
    m_camera->resize(event.width, event.height);
    for (int i = 0; i < 2; ++i) {
        m_lightTarget[i].resize(event.width, event.height);
        m_blurTarget[i].resize(event.width, event.height);
    }
    m_gBufferTarget.resize(event.width, event.height);
    m_gBuffer->resize(event.width, event.height);
}

void RenderSystem::onSceneEvent(const SceneEvent &event) {
    m_scene = event.scene;
}

void RenderSystem::onTick(float) {}

void RenderSystem::onRender() {
    SD_CORE_ASSERT(m_scene, "No active scene.");
    SD_CORE_ASSERT(m_camera, "No camera is set!");

    renderGBuffer();
    renderShadow();
    renderLight();
    renderEmissive();
    renderText();
    if (m_isBloom) {
        renderBlur();
    }
    renderMain();
}

void RenderSystem::renderMain() {
    Renderer::setRenderTarget(Application::getRenderEngine().getRenderTarget());
    Device::instance().clear();

    m_mainShader->bind();
    m_mainShader->setBool("u_bloom", m_isBloom);
    if (m_isBloom) {
        m_mainShader->setFloat("u_bloomFactor", m_bloom);
        m_mainShader->setTexture("u_blur", m_blurResult);
    }
    m_mainShader->setTexture("u_lighting", getLightResult().getTexture());
    m_mainShader->setFloat("u_exposure", m_exposure);
    Renderer::submit(*m_quad, MeshTopology::TRIANGLES, 6, 0);

    getLightResult().getFramebuffer()->copyTo(
        Application::getRenderEngine().getRenderTarget().getFramebuffer(), 0,
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
        Renderer::submit(*m_quad, MeshTopology::TRIANGLES, 6, 0);
        horizontal = !horizontal;
    }
}

void RenderSystem::renderText() {
    Renderer::setRenderTarget(getLightResult());
    Renderer::beginScene(*m_camera);
    auto textView = m_scene->view<TransformComponent, TextComponent>();
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
    auto shader = Asset::manager().load<Shader>("shaders/emissive.glsl");
    shader->bind();
    shader->setTexture("u_lighting", getLightResult().getTexture());
    shader->setTexture("u_gEmissive", getGBuffer()->getTexture(
                                          GeometryBufferType::G_EMISSIVE));
    Device::instance().setDepthMask(false);
    Renderer::submit(*m_quad, MeshTopology::TRIANGLES, 6, 0);
    Device::instance().setDepthMask(true);
}

void RenderSystem::renderShadow() {
    auto lightView = m_scene->view<TransformComponent, LightComponent>();
    auto modelView = m_scene->view<TransformComponent, ModelComponent>();
    m_shadowShader->bind();

    Device::instance().setCullFace(Face::FRONT);
    lightView.each([this, &modelView](const TransformComponent &transformComp,
                                      LightComponent &lightComp) {
        Light &light = lightComp.light;
        if (!light.isCastShadow()) return;

        Renderer::setRenderTarget(light.getRenderTarget());
        Device::instance().clear(BufferBitMask::DEPTH_BUFFER_BIT);
        light.computeLightSpaceMatrix(transformComp.transform, m_camera);
        m_shadowShader->setMat4("u_projectionView", light.getProjectionView());

        modelView.each([this](const TransformComponent &transformComp,
                              const ModelComponent &modelComp) {
            m_shadowShader->setMat4(
                "u_model", transformComp.transform.getWorldTransform());
            for (const auto &mesh : modelComp.model->getMeshes()) {
                Renderer::drawMesh(mesh);
            }
        });
    });
    Device::instance().setCullFace(Face::BACK);
}

void RenderSystem::renderLight() {
    Device::instance().setDepthMask(false);
    Device::instance().disable(Operation::DEPTH_TEST);
    Renderer::setShader(*m_lightShader);
    Framebuffer *gBuffer = getGBuffer();
    m_lightShader->setTexture(
        "u_gPosition", gBuffer->getTexture(GeometryBufferType::G_POSITION));
    m_lightShader->setTexture(
        "u_gNormal", gBuffer->getTexture(GeometryBufferType::G_NORMAL));
    m_lightShader->setTexture(
        "u_gAlbedo", gBuffer->getTexture(GeometryBufferType::G_ALBEDO));
    m_lightShader->setTexture(
        "u_gAmbient", gBuffer->getTexture(GeometryBufferType::G_AMBIENT));
    const uint8_t inputIndex = 0;
    const uint8_t outputIndex = 1;

    // clear the last lighting pass' result
    const float color[] = {0, 0, 0, 1.0};
    getLightResult().getFramebuffer()->clearAttachment(0, color);

    auto lightView = m_scene->view<TransformComponent, LightComponent>();
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
        Renderer::submit(*m_quad, MeshTopology::TRIANGLES, 6, 0);
        std::swap(m_lightTarget[outputIndex], m_lightTarget[inputIndex]);
    });

    Device::instance().setDepthMask(true);
    Device::instance().enable(Operation::DEPTH_TEST);

    getGBuffer()->copyTo(getLightResult().getFramebuffer(), 0,
                         BufferBitMask::DEPTH_BUFFER_BIT,
                         TextureFilter::NEAREST);
}

void RenderSystem::renderGBuffer() {
    Device::instance().disable(Operation::BLEND);
    Renderer::setRenderTarget(m_gBufferTarget);
    Device::instance().setClearColor(0.f, 0.f, 0.f, 1.0);
    Device::instance().clear();

    Renderer::beginScene(*m_camera);
    Renderer::setShader(*m_gBufferShader);
    m_gBufferTarget.getFramebuffer()->clearAttachment(
        GeometryBufferType::G_ENTITY_ID, &Entity::INVALID_ID);

    m_gBufferShader->bind();
    auto terrainView = m_scene->view<TransformComponent, TerrainComponent>();
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

    auto modelView = m_scene->view<TransformComponent, ModelComponent>();
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

    for (int i = 0; i < GeometryBufferType::GBUFFER_COUNT; ++i) {
        m_gBufferTarget.getFramebuffer()->copyTo(
            getGBuffer(), i,
            BufferBitMask::COLOR_BUFFER_BIT | BufferBitMask::DEPTH_BUFFER_BIT,
            TextureFilter::NEAREST);
    }
    Renderer::endScene();

    Device::instance().enable(Operation::BLEND);
}

void RenderSystem::setCamera(Camera *camera) { m_camera = camera; }

Camera *RenderSystem::getCamera() { return m_camera; }

void RenderSystem::setExposure(float exposure) { m_exposure = exposure; }

float RenderSystem::getExposure() const { return m_exposure; }

void RenderSystem::setBloom(bool isBloom) { m_isBloom = isBloom; }

bool RenderSystem::getBloom() const { return m_isBloom; }

void RenderSystem::setBloomFactor(float bloom) { m_bloom = bloom; }

float RenderSystem::getBloomFactor() const { return m_bloom; }

}  // namespace sd
