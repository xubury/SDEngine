#include "Sandbox2DLayer.hpp"
#include "Core/Application.hpp"
#include "Graphics/Device.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Renderer/Renderer3D.hpp"
#include "Renderer/Renderer.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/Texture.hpp"
#include "Common/Log.hpp"
#include "Graphics/Graphics.hpp"
#include "Core/InputManager.hpp"
#include "Utils/Random.hpp"

Sandbox2DLayer::Sandbox2DLayer()
    : sd::Layer("Sandbox2D"),
      m_actionTarget(m_actionMap),
      m_masterCam(800.f, 600.f, -1.0f, 1.0f) {
    m_actionMap.map(0, SDLK_a);
    m_actionMap.map(
        1, sd::Action(SDLK_b, sd::Action::REAL_TIME | sd::Action::DOWN));
    m_actionTarget.bind(0,
                        [](const SDL_Event &) { SD_TRACE("Key Pressed:A"); });
    m_actionTarget.bind(1,
                        [](const SDL_Event &) { SD_TRACE("Key Pressed:B"); });

    m_particleSystem = m_systems.addSystem<sd::ParticleSystem>(10000);
}

void Sandbox2DLayer::onAttach() {}

void Sandbox2DLayer::onTick(float dt) { m_systems.tick(dt); }

void Sandbox2DLayer::onRender() {
    sd::Renderer2D::beginScene(m_masterCam, nullptr);
    sd::Renderer::setClearColor(0.1, 0.2, 0.3, 1.0);
    sd::Renderer::clear();
    m_systems.render();
    sd::Renderer2D::drawTexture(
        glm::scale(glm::mat4(1.0f), glm::vec3(80.f, 24.f, 1.0f)),
        sd::Graphics::assetManager().load<sd::Texture>(
            "textures/1_diagdown.png"));
    sd::Renderer2D::endScene();
}

void Sandbox2DLayer::onEventPoll(const SDL_Event &event) {
    switch (event.type) {
        case SDL_MOUSEMOTION: {
            sd::ParticleProp prop;
            glm::vec2 view = sd::Renderer::getDefaultTarget()->mapScreenToClip(
                sd::InputManager::getMouseCoord());
            glm::vec3 world = m_masterCam.mapClipToWorld(view);
            world.z = -1.0f;
            for (int i = 0; i < 10; ++i) {
                prop.position =
                    world + glm::vec3(sd::Random::rnd(-10.f, 10.f),
                                      sd::Random::rnd(-10.f, 10.f), 0.5f);
                prop.velocity = glm::vec3(0.f, 50.f, 0.f);
                prop.velocityVariation = glm::vec3(50.f, 10.f, 0.f);
                prop.colorBegin = glm::vec4(0.5, 0.5, 0.f, 1.0f);
                prop.colorEnd = glm::vec4(0.f, 0.5, 5.f, 1.0f);
                prop.sizeBegin = 10.0f;
                prop.sizeEnd = 2.f;
                prop.sizeVariation = 1.f;
                prop.lifeTime = 2.0f;
                m_particleSystem->emit(prop);
            }
        } break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                sd::Application::instance().quit();
            break;
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                sd::Renderer::getDefaultTarget()->resize(event.window.data1,
                                                         event.window.data2);
            }
            break;
    }
    m_actionTarget.processEvent(event);
}

void Sandbox2DLayer::onEventProcess() { m_actionTarget.processEvents(); }
