#include "Renderer/System/PostProcessSystem.hpp"
#include "Renderer/RenderEngine.hpp"
#include "Graphics/Device.hpp"

namespace sd {

PostProcessSystem::PostProcessSystem(int width, int height) {
    for (int i = 0; i < 2; ++i) {
        m_blurTarget[i].addTexture(Texture::create(
            width, height, 1, TextureType::TEX_2D, TextureFormat::RGBA,
            TextureFormatType::FLOAT, TextureWrap::BORDER,
            TextureFilter::LINEAR, TextureMipmapFilter::LINEAR));
        m_blurTarget[i].createFramebuffer();
    }
    m_blurShader = Asset::manager().load<Shader>("shaders/blur.glsl");
    m_mainShader = Asset::manager().load<Shader>("shaders/main.glsl");

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

void PostProcessSystem::onInit() {
    registerEvent(this, &PostProcessSystem::onSizeEvent);
}

void PostProcessSystem::onDestroy() { unregisterEvent<SizeEvent>(this); }

void PostProcessSystem::onRender() {
    renderSkybox();
    if (RenderEngine::getBloom()) {
        renderBlur();
    }
    renderMain();
}

void PostProcessSystem::onSizeEvent(const SizeEvent &event) {
    for (int i = 0; i < 2; ++i) {
        m_blurTarget[i].resize(event.width, event.height);
    }
}

void PostProcessSystem::renderSkybox() {
    glm::vec3 pos = RenderEngine::getCamera()->getWorldPosition();
    m_skyboxShader->setMat4("u_model", glm::translate(glm::mat4(1.0f), pos));

    m_skyboxShader->bind();
    Device::instance().setDepthfunc(DepthFunc::LESS_EQUAL);
    Device::instance().setCullFace(Face::FRONT);

    Device::instance().setRenderTarget(RenderEngine::getRenderTarget());
    Device::instance().submit(*m_skybox, MeshTopology::TRIANGLES,
                              m_skybox->getIndexBuffer()->getCount(), 0);
    Device::instance().setCullFace(Face::BACK);
    Device::instance().setDepthfunc(DepthFunc::LESS);
}

void PostProcessSystem::renderBlur() {
    const int amount = 10;
    bool horizontal = true;
    m_blurShader->bind();
    for (int i = 0; i < amount; ++i) {
        const int inputId = horizontal;
        const int outputId = !horizontal;
        Device::instance().setRenderTarget(m_blurTarget[outputId]);
        m_blurResult = m_blurTarget[outputId].getTexture();
        m_blurShader->setBool("u_horizontal", horizontal);
        m_blurShader->setTexture(
            "u_image", i == 0 ? RenderEngine::getRenderTarget().getTexture()
                              : m_blurTarget[inputId].getTexture());
        RenderEngine::renderQuad();
        horizontal = !horizontal;
    }
}

void PostProcessSystem::renderMain() {
    Device::instance().setRenderTarget(RenderEngine::getRenderTarget());

    m_mainShader->setBool("u_bloom", RenderEngine::getBloom());
    if (RenderEngine::getBloom()) {
        m_mainShader->setFloat("u_bloomFactor", RenderEngine::getBloomFactor());
        m_mainShader->setTexture("u_blur", m_blurResult);
    }
    m_mainShader->setTexture("u_lighting",
                             RenderEngine::getRenderTarget().getTexture());
    m_mainShader->setFloat("u_exposure", RenderEngine::getExposure());
    m_mainShader->bind();
    Device::instance().setDepthMask(false);
    RenderEngine::renderQuad();
    Device::instance().setDepthMask(true);
}

}  // namespace sd
