#include "Renderer/System/PostProcessSystem.hpp"
#include "Renderer/Renderer.hpp"

namespace SD {

PostProcessSystem::PostProcessSystem(RenderTarget *target, int width,
                                     int height)
    : System("PostProcess"),
      m_target(target),
      m_isBloom(true),
      m_bloom(1.0f),
      m_exposure(1.2),
      m_gammaCorrection(1.2) {
    for (int i = 0; i < 2; ++i) {
        m_blurTarget[i].addTexture(Texture::create(
            width, height, 1, TextureType::TEX_2D, TextureFormat::RGBA,
            TextureFormatType::FLOAT, TextureWrap::BORDER,
            TextureFilter::LINEAR, TextureMipmapFilter::LINEAR));
        m_blurTarget[i].createFramebuffer();
    }
    m_postTarget.addTexture(Texture::create(
        width, height, 1, TextureType::TEX_2D, TextureFormat::RGBA,
        TextureFormatType::FLOAT, TextureWrap::EDGE, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR));
    m_postTarget.createFramebuffer();
    m_blurShader = ShaderLibrary::instance().load("shaders/blur.glsl");
    m_postShader = ShaderLibrary::instance().load("shaders/postProcess.glsl");
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

void PostProcessSystem::onPush() {
    dispatcher->subscribe(this, &PostProcessSystem::onSizeEvent);
}

void PostProcessSystem::onPop() { dispatcher->unsubscribe<WindowSizeEvent>(this); }

void PostProcessSystem::onRender() {
    renderer->device().setDepthMask(false);
    renderer->device().blitFramebuffer(
        m_target->getFramebuffer(), 0, m_postTarget.getFramebuffer(), 0,
        BufferBitMask::COLOR_BUFFER_BIT, TextureFilter::LINEAR);
    if (m_isBloom) {
        renderBlur();
    }
    renderPost();
    renderer->device().setDepthMask(true);
}

void PostProcessSystem::onSizeEvent(const WindowSizeEvent &event) {
    for (int i = 0; i < 2; ++i) {
        m_blurTarget[i].resize(event.width, event.height);
    }
    m_postTarget.resize(event.width, event.height);
}

void PostProcessSystem::renderBlur() {
    const int amount = 10;
    bool horizontal = true;
    m_blurShader->bind();
    for (int i = 0; i < amount; ++i) {
        const int inputId = horizontal;
        const int outputId = !horizontal;
        renderer->setRenderTarget(m_blurTarget[outputId]);
        m_blurResult = m_blurTarget[outputId].getTexture();
        m_blurShader->setBool("u_horizontal", horizontal);
        m_blurShader->setTexture("u_image",
                                 i == 0 ? m_postTarget.getTexture()
                                        : m_blurTarget[inputId].getTexture());
        renderer->submit(*m_quad, MeshTopology::TRIANGLES,
                         m_quad->getIndexBuffer()->getCount(), 0);
        horizontal = !horizontal;
    }
}

void PostProcessSystem::renderPost() {
    renderer->setRenderTarget(*m_target);

    m_postShader->setBool("u_bloom", m_isBloom);
    m_postShader->setFloat("u_bloomFactor", m_bloom);
    m_postShader->setTexture("u_blur", m_blurResult);

    m_postShader->setTexture("u_lighting", m_postTarget.getTexture());
    m_postShader->setFloat("u_exposure", m_exposure);

    m_postShader->setFloat("u_gamma", m_gammaCorrection);

    m_postShader->bind();
    renderer->submit(*m_quad, MeshTopology::TRIANGLES,
                     m_quad->getIndexBuffer()->getCount(), 0);
}

void PostProcessSystem::setExposure(float exposure) { m_exposure = exposure; }

float PostProcessSystem::getExposure() { return m_exposure; }

void PostProcessSystem::setBloom(bool isBloom) { m_isBloom = isBloom; }

bool PostProcessSystem::getBloom() { return m_isBloom; }

void PostProcessSystem::setBloomFactor(float bloom) { m_bloom = bloom; }

float PostProcessSystem::getBloomFactor() { return m_bloom; }

void PostProcessSystem::setGammaCorrection(float gamma) {
    m_gammaCorrection = gamma;
}

float PostProcessSystem::getGammaCorrection() { return m_gammaCorrection; }

}  // namespace SD
