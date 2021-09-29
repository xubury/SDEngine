#include "ECS/RenderSystem.hpp"
#include "Graphics/Device.hpp"
#include "Renderer/Renderer3D.hpp"
#include "Renderer/RenderEngine.hpp"
#include "Utils/Log.hpp"
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
        linear ? TextureMipmapFilter::LINEAR_NEAREST
               : TextureMipmapFilter::NEAREST);
}

inline TextureFormat getTextureFormat(RenderSystem::GBufferType type) {
    switch (type) {
        case RenderSystem::G_POSITION:
        case RenderSystem::G_NORMAL:
        case RenderSystem::G_ALBEDO:
        case RenderSystem::G_AMBIENT:
            return TextureFormat::RGBA;
        case RenderSystem::G_ENTITY_ID:
            return TextureFormat::ALPHA;
        default:
            SD_CORE_WARN("Unknown GBuffer!");
            return TextureFormat::RGBA;
    }
}

inline TextureFormatType getTextureFormatType(RenderSystem::GBufferType type) {
    switch (type) {
        case RenderSystem::G_POSITION:
        case RenderSystem::G_NORMAL:
            return TextureFormatType::FLOAT;
        case RenderSystem::G_ALBEDO:
        case RenderSystem::G_AMBIENT:
            return TextureFormatType::UBYTE;
        case RenderSystem::G_ENTITY_ID:
            return TextureFormatType::UINT;
        default:
            SD_CORE_WARN("Unknown GBuffer!");
            return TextureFormatType::UBYTE;
    }
}

RenderSystem::RenderSystem(RenderEngine *engine, int width, int height,
                           int samples)
    : m_engine(engine), m_blurResult(nullptr), m_lightResult(nullptr) {
    Device::instance().setMultisample(samples > 1);

    m_mainShader = Asset::manager().load<Shader>("shaders/main.glsl");
    m_lightShader = Asset::manager().load<Shader>("shaders/lighting.glsl");
    m_blurShader = Asset::manager().load<Shader>("shaders/blur.glsl");

    for (int i = 0; i < 2; ++i) {
        m_blurBuffer[i] = Framebuffer::create();
        m_blurBuffer[i]->attachTexture(
            createTexture(width, height, 1, TextureFormat::RGBA,
                          TextureFormatType::UBYTE, true));
        m_lightBuffer[i] = Framebuffer::create();
        m_lightBuffer[i]->attachTexture(
            createTexture(width, height, 1, TextureFormat::RGBA,
                          TextureFormatType::FLOAT, true));
    }

    initQuad();
    initGBuffer(width, height, samples);
}

void RenderSystem::initQuad() {
    float quadVertices[] = {
        1.0f,  1.0f,  0.0f, 1.0f, 1.0f,  // top right
        1.0f,  -1.0f, 0.0f, 1.0f, 0.f,   // bottom right
        -1.0f, -1.0f, 0.0f, 0.f,  0.f,   // bottom left
        -1.0f, 1.0f,  0.0f, 0.f,  1.0f   // top left
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
    m_gBuffer = Framebuffer::create();
    for (int i = 0; i < GBUFFER_COUNT; ++i) {
        TextureFormat format = getTextureFormat(GBufferType(i));
        TextureFormatType type = getTextureFormatType(GBufferType(i));
        bool linear = i != G_ENTITY_ID;
        m_gBufferTarget.addTexture(
            createTexture(width, height, samples, format, type, linear));
        m_gBuffer->attachTexture(
            createTexture(width, height, 1, format, type, linear));
    }
    // depth
    m_gBufferTarget.addTexture(Texture::create(
        width, height, samples,
        samples > 1 ? TextureType::TEX_2D_MULTISAMPLE : TextureType::TEX_2D,
        TextureFormat::DEPTH, TextureFormatType::FLOAT, TextureWrap::BORDER,
        TextureFilter::LINEAR, TextureMipmapFilter::LINEAR_NEAREST));
    m_gBufferTarget.init();
}

void RenderSystem::resize(int width, int height) {
    for (int i = 0; i < 2; ++i) {
        m_lightBuffer[i]->resize(width, height);
        m_blurBuffer[i]->resize(width, height);
    }
    m_gBufferTarget.resize(width, height);
    m_gBuffer->resize(width, height);
}

void RenderSystem::onTick(float) {}

void RenderSystem::onRender() {
    renderGBuffer();
    renderLight();
    if (m_engine->getBloom()) {
        renderBlur();
    }
    renderMain();
}

void RenderSystem::renderMain() {
    m_engine->getRenderTarget()->use();
    Framebuffer *gBuffer = getGBuffer();
    Device::instance().clear();
    m_mainShader->bind();
    bool bloom = m_engine->getBloom();
    m_mainShader->setBool("u_bloom", bloom);
    if (bloom) {
        m_mainShader->setFloat("u_bloomFactor", m_engine->getBloomFactor());
        m_mainShader->setTexture("u_blur", m_blurResult);
    }
    m_mainShader->setTexture("u_lighting", m_lightResult);
    m_mainShader->setTexture("u_gEntityId", gBuffer->getTexture(G_ENTITY_ID));
    m_mainShader->setFloat("u_exposure", m_engine->getExposure());
    Renderer::submit(*m_quad, MeshTopology::TRIANGLES, 6, 0);
}

void RenderSystem::renderBlur() {
    const int amount = 10;
    bool horizontal = true;
    m_blurShader->bind();
    for (int i = 0; i < amount; ++i) {
        const int inputId = horizontal;
        const int outputId = !horizontal;
        m_blurBuffer[outputId]->bind();
        m_blurResult = m_blurBuffer[outputId]->getTexture();
        m_blurShader->setBool("u_horizontal", horizontal);
        m_blurShader->setTexture(
            "u_image",
            i == 0 ? m_lightResult : m_blurBuffer[inputId]->getTexture());
        Renderer::submit(*m_quad, MeshTopology::TRIANGLES, 6, 0);
        horizontal = !horizontal;
    }
}

void RenderSystem::renderLight() {
    m_lightShader->bind();
    Framebuffer *gBuffer = getGBuffer();
    m_lightShader->setTexture("u_gPosition", gBuffer->getTexture(G_POSITION));
    m_lightShader->setTexture("u_gNormal", gBuffer->getTexture(G_NORMAL));
    m_lightShader->setTexture("u_gAlbedo", gBuffer->getTexture(G_ALBEDO));
    m_lightShader->setTexture("u_gAmbient", gBuffer->getTexture(G_AMBIENT));
    Device::instance().setDepthMask(false);
    const uint8_t inputIndex = 0;
    const uint8_t outputIndex = 1;

    // clear the last lighting pass' result
    const float color[] = {0, 0, 0, 1.0};
    m_lightBuffer[inputIndex]->clearAttachment(0, color);

    auto lightView = m_scene->view<TransformComponent, LightComponent>();
    lightView.each([this](const TransformComponent &transformComp,
                          const LightComponent &lightComp) {
        m_lightBuffer[outputIndex]->bind();
        m_lightResult = m_lightBuffer[outputIndex]->getTexture();
        m_lightShader->setTexture("u_lighting",
                                  m_lightBuffer[inputIndex]->getTexture());
        const Transform &transform = transformComp.transform;
        const Light &light = lightComp.light;
        m_lightShader->setVec3("u_light.direction", transform.getWorldFront());
        m_lightShader->setVec3("u_light.ambient", light.ambient);
        m_lightShader->setVec3("u_light.diffuse", light.diffuse);
        m_lightShader->setVec3("u_light.specular", light.specular);
        m_lightShader->setVec3("u_light.position",
                               transform.getWorldPosition());
        m_lightShader->setFloat("u_light.isDirectional", light.isDirectional);
        m_lightShader->setFloat("u_light.cutOff",
                                glm::cos(glm::radians(light.cutOff)));
        m_lightShader->setFloat("u_light.outerCutOff",
                                glm::cos(glm::radians(light.outerCutOff)));
        m_lightShader->setFloat("u_light.constant", light.constant);
        m_lightShader->setFloat("u_light.linear", light.linear);
        m_lightShader->setFloat("u_light.quadratic", light.quadratic);
        Renderer::submit(*m_quad, MeshTopology::TRIANGLES, 6, 0);
        std::swap(m_lightBuffer[outputIndex], m_lightBuffer[inputIndex]);
    });

    Device::instance().setDepthMask(true);
}

void RenderSystem::renderGBuffer() {
    Renderer3D::beginScene(*m_engine->getCamera(), *m_gBufferShader);
    m_gBufferTarget.use();
    Device::instance().setBlend(false);
    Device::instance().setClearColor(0.f, 0.f, 0.f, 1.0);
    Device::instance().clear();
    m_gBufferTarget.getFramebuffer()->clearAttachment(G_ENTITY_ID,
                                                      &Entity::INVALID_ID);
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
            Renderer3D::drawMesh(mesh);
        }
    });
    m_gBuffer->copyFrom(m_gBufferTarget.getFramebuffer(),
                        BufferBit::COLOR_BUFFER_BIT, TextureFilter::NEAREST);
    Device::instance().setBlend(true);
    Renderer3D::endScene();
}

Framebuffer *RenderSystem::getGBuffer() { return m_gBuffer.get(); }

}  // namespace sd
