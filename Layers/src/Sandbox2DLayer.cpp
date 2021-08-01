#include "Sandbox2DLayer.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Renderer2D.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/Texture.hpp"
#include "Utils/Log.hpp"
#include "Graphics/GraphicsManager.hpp"

Sandbox2DLayer::Sandbox2DLayer() : sd::Layer("Sandbox2D") {}

void Sandbox2DLayer::onAttach() {}

void Sandbox2DLayer::onTick(float) {}

void Sandbox2DLayer::onRender() {
    sd::Renderer::setClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    sd::Renderer::clear();
    sd::Renderer2D::beginScene(sd::OrthographicCamera(3, 3, -1.0f, 1.0f));
    sd::Renderer2D::drawTexture(
        glm::mat4(1.0f),
        sd::GraphicsManager::textures().load<sd::Texture>("image.png"));
    sd::Renderer2D::drawQuad(
        glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 1.0f)),
        glm::vec4(1, 0, 0, 0.5));
    sd::Renderer2D::endScene();
}

void Sandbox2DLayer::onEventPoll(const SDL_Event &) {}

void Sandbox2DLayer::onEventProcess() {}
