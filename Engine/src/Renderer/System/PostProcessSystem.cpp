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
    m_postShader = Asset::manager().load<Shader>("shaders/postProcess.glsl");
}

void PostProcessSystem::onInit() {
    registerEvent(this, &PostProcessSystem::onSizeEvent);
}

void PostProcessSystem::onDestroy() { unregisterEvent<SizeEvent>(this); }

void PostProcessSystem::onRender() {
    Device::instance().setDepthMask(false);
    if (RenderEngine::getBloom()) {
        renderBlur();
    }
    // Device::instance().blitFramebuffer(
    //     RenderEngine::getRenderTarget().getFramebuffer(), 0, nullptr, 0,
    //     BufferBitMask::COLOR_BUFFER_BIT, TextureFilter::LINEAR);
    renderPost();
    Device::instance().setDepthMask(true);
}

void PostProcessSystem::onSizeEvent(const SizeEvent &event) {
    for (int i = 0; i < 2; ++i) {
        m_blurTarget[i].resize(event.width, event.height);
    }
}

void PostProcessSystem::renderBlur() {
    const int amount = 10;
    bool horizontal = true;
    int inputId = horizontal;
    int outputId = !horizontal;
    m_blurShader->bind();
    Device::instance().blitFramebuffer(
        RenderEngine::getRenderTarget().getFramebuffer(), 0,
        m_blurTarget[inputId].getFramebuffer(), 0,
        BufferBitMask::COLOR_BUFFER_BIT, TextureFilter::LINEAR);
    for (int i = 0; i < amount; ++i) {
        inputId = horizontal;
        outputId = !horizontal;
        m_blurTarget[outputId].bind();
        m_blurResult = m_blurTarget[outputId].getTexture();
        m_blurShader->setBool("u_horizontal", horizontal);
        m_blurShader->setTexture("u_image", m_blurTarget[inputId].getTexture());
        RenderEngine::renderQuad();
        horizontal = !horizontal;
    }
}

void PostProcessSystem::renderPost() {
    RenderEngine::getRenderTarget().bind();

    m_postShader->setBool("u_bloom", RenderEngine::getBloom());
    m_postShader->setFloat("u_bloomFactor", RenderEngine::getBloomFactor());
    m_postShader->setTexture("u_blur", m_blurResult);

    m_postShader->setTexture("u_lighting",
                             RenderEngine::getRenderTarget().getTexture());
    m_postShader->setFloat("u_exposure", RenderEngine::getExposure());

    m_postShader->setFloat("u_gamma", RenderEngine::getGammaCorrection());

    m_postShader->bind();
    RenderEngine::renderQuad();
}

}  // namespace sd
