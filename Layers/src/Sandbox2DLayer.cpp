#include "Sandbox2DLayer.hpp"
#include <GL/glew.h>
#include "Graphics/Renderer2D.hpp"
#include "Graphics/Camera.hpp"

Sandbox2DLayer::Sandbox2DLayer() : sd::Layer("Sandbox2D") {}

void Sandbox2DLayer::onTick(float) {}

void Sandbox2DLayer::onRender() {
    glClearColor(0.1, 0.2, 0.3, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, 800, 600);
    sd::Renderer2D::beginScene(sd::OrthographicCamera(3, 3, -1.0f, 1.0f));
    sd::Renderer2D::drawQuad(glm::mat4(1.0f), glm::vec4(1, 0, 0, 0));
    sd::Renderer2D::endScene();
}

void Sandbox2DLayer::onEventPoll(const SDL_Event &) {}

void Sandbox2DLayer::onEventProcess() {}
