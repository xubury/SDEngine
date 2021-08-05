#include "Sandbox2DLayer.hpp"
#include "Core/Application.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Renderer2D.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/Texture.hpp"
#include "Utils/Log.hpp"
#include "Graphics/Graphics.hpp"

Sandbox2DLayer::Sandbox2DLayer() : sd::Layer("Sandbox2D") {}

void Sandbox2DLayer::onAttach() {
    m_texture = sd::Texture::create(
        1024, 1024, sd::TextureType::TEX_2D, sd::TextureFormat::RGBA,
        sd::TextureFormatType::UBYTE, sd::TextureWrap::BORDER,
        sd::TextureFilter::NEAREST, sd::TextureMipmapFilter::NEAREST);
    m_target.addTexture(m_texture);
    m_target.addTexture(sd::Texture::create(
        m_texture->getWidth(), m_texture->getHeight(), sd::TextureType::TEX_2D,
        sd::TextureFormat::DEPTH, sd::TextureFormatType::FLOAT,
        sd::TextureWrap::BORDER, sd::TextureFilter::NEAREST,
        sd::TextureMipmapFilter::NEAREST));
    m_target.init();

    m_defaultTarget.resize(sd::Application::getWindow().getWidth(),
                           sd::Application::getWindow().getHeight());
}

void Sandbox2DLayer::onTick(float) {}

void Sandbox2DLayer::onRender() {
    sd::Renderer2D::beginScene(sd::OrthographicCamera(3, 3, -1.0f, 1.0f),
                               m_target);
    sd::Renderer::setClearColor(0.f, 0.f, 0.f, 0.0f);
    sd::Renderer::clear();
    sd::Renderer2D::drawTexture(
        glm::mat4(1.0f),
        sd::Graphics::assetManager().load<sd::Texture>("textures/image.png"));
    sd::Renderer2D::drawQuad(
        glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 1.0f)),
        glm::vec4(0.f, 1.f, 0, 1.0f));
    sd::Renderer2D::endScene();

    sd::Renderer2D::beginScene(sd::OrthographicCamera(1, 1, -1.0f, 1.0f),
                               m_defaultTarget);
    sd::Renderer::setClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    sd::Renderer::clear();
    sd::Renderer2D::drawTexture(glm::mat4(1.0f), m_texture);
    sd::Renderer2D::endScene();
}

void Sandbox2DLayer::onEventPoll(const SDL_Event &event) {
    switch (event.type) {
        case SDL_WINDOWEVENT: {
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                m_defaultTarget.resize(event.window.data1, event.window.data2);
            }
        }
    }
}

void Sandbox2DLayer::onEventProcess() {}
