#include "Renderer/System/PostProcessSystem.hpp"
#include "Renderer/Renderer.hpp"

namespace SD {

PostProcessSystem::PostProcessSystem(int width, int height) {
    for (int i = 0; i < 2; ++i) {
        m_blurTarget[i].addTexture(Texture::create(
            width, height, 1, TextureType::TEX_2D, TextureFormat::RGBA,
            TextureFormatType::FLOAT, TextureWrap::BORDER,
            TextureFilter::LINEAR, TextureMipmapFilter::LINEAR));
        m_blurTarget[i].createFramebuffer();
    }
    m_target.addTexture(Texture::create(
        width, height, 1, TextureType::TEX_2D, TextureFormat::RGBA,
        TextureFormatType::FLOAT, TextureWrap::EDGE, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR));
    m_target.createFramebuffer();
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

void PostProcessSystem::onInit() {
    registerEvent(this, &PostProcessSystem::onSizeEvent);
}

void PostProcessSystem::onDestroy() { unregisterEvent<SizeEvent>(this); }

void PostProcessSystem::onRender() {
    Renderer::device().setDepthMask(false);
    Renderer::device().blitFramebuffer(
        Renderer::engine().getRenderTarget().getFramebuffer(), 0,
        m_target.getFramebuffer(), 0, BufferBitMask::COLOR_BUFFER_BIT,
        TextureFilter::LINEAR);
    if (Renderer::engine().getBloom()) {
        renderBlur();
    }
    renderPost();
    Renderer::device().setDepthMask(true);
}

void PostProcessSystem::onSizeEvent(const SizeEvent &event) {
    for (int i = 0; i < 2; ++i) {
        m_blurTarget[i].resize(event.width, event.height);
    }
    m_target.resize(event.width, event.height);
}

void PostProcessSystem::renderBlur() {
    const int amount = 10;
    bool horizontal = true;
    m_blurShader->bind();
    for (int i = 0; i < amount; ++i) {
        const int inputId = horizontal;
        const int outputId = !horizontal;
        m_blurTarget[outputId].bind();
        m_blurResult = m_blurTarget[outputId].getTexture();
        m_blurShader->setBool("u_horizontal", horizontal);
        m_blurShader->setTexture("u_image",
                                 i == 0 ? m_target.getTexture()
                                        : m_blurTarget[inputId].getTexture());
        Renderer::submit(*m_quad, MeshTopology::TRIANGLES,
                         m_quad->getIndexBuffer()->getCount(), 0);
        horizontal = !horizontal;
    }
}

void PostProcessSystem::renderPost() {
    Renderer::engine().getRenderTarget().bind();

    m_postShader->setBool("u_bloom", Renderer::engine().getBloom());
    m_postShader->setFloat("u_bloomFactor",
                           Renderer::engine().getBloomFactor());
    m_postShader->setTexture("u_blur", m_blurResult);

    m_postShader->setTexture("u_lighting", m_target.getTexture());
    m_postShader->setFloat("u_exposure", Renderer::engine().getExposure());

    m_postShader->setFloat("u_gamma", Renderer::engine().getGammaCorrection());

    m_postShader->bind();
    Renderer::submit(*m_quad, MeshTopology::TRIANGLES,
                     m_quad->getIndexBuffer()->getCount(), 0);
}

}  // namespace SD
