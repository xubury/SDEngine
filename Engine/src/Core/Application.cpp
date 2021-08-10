#include "Core/Application.hpp"
#include "Common/Log.hpp"
#include <SDL_image.h>
#include "Utils/Random.hpp"
#include "Core/InputManager.hpp"
#include "Core/Timing.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Renderer/Renderer3D.hpp"

namespace sd {

Application *Application::s_instance = nullptr;

Application &Application::instance() { return *s_instance; }

Application::Application() {
    std::string debugPath = "Debug.txt";
    Log::init(debugPath);
    int width = 800;
    int height = 600;
    Random::init();
    SD_CORE_INFO("Debug info is output to: {}", debugPath);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        SD_CORE_ERROR("SDL_Init failed: {}", SDL_GetError());
        exit(-1);
    }

    int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
        SD_CORE_ERROR("IMG_Init Failed: {}", IMG_GetError());
        exit(-1);
    }

    sd::Graphics::init(sd::API::OpenGL);

    if (!m_window.create("SD Engine", width, height, Window::WINDOWED)) {
        exit(-1);
    }

    s_instance = this;

    sd::Renderer::init();

    sd::Renderer2D::init();
    sd::Renderer3D::init();

    sd::Renderer::getDefaultTarget()->resize(width, height);
    m_imguiLayer = new ImGuiLayer();
    pushOverlay(m_imguiLayer);
}

Application::~Application() {
    IMG_Quit();
    SDL_Quit();
}

void Application::pushLayer(Layer *layer) { m_layers.pushLayer(layer); }

void Application::pushOverlay(Layer *layer) { m_layers.pushOverlay(layer); }

void Application::popLayer(Layer *layer) { m_layers.popLayer(layer); }

void Application::popOverlay(Layer *layer) { m_layers.popOverlay(layer); }

void Application::onEventPoll(const SDL_Event &event) {
    switch (event.type) {
        case SDL_QUIT:
            quit();
            break;
        case SDL_KEYDOWN:
            InputManager::instance().pressKey(event.key.keysym.sym);
            break;
        case SDL_KEYUP:
            InputManager::instance().releaseKey(event.key.keysym.sym);
            break;
        case SDL_MOUSEBUTTONDOWN:
            InputManager::instance().pressMouseButton(event.button.button);
            break;
        case SDL_MOUSEBUTTONUP:
            InputManager::instance().releaseMouseButton(event.button.button);
            break;
        case SDL_MOUSEMOTION:
            InputManager::instance().setMouseCoord(event.motion.x,
                                                   event.motion.y);
            break;
        default:
            break;
    }
    for (auto layer = m_layers.rbegin(); layer != m_layers.rend(); ++layer) {
        (*layer)->onEventPoll(event);
        if ((*layer)->isBlockEvent()) break;
    }
}

void Application::onEventProcess() {
    for (auto layer = m_layers.rbegin(); layer != m_layers.rend(); ++layer) {
        (*layer)->onEventProcess();
        if ((*layer)->isBlockEvent()) break;
    }
}

void Application::run() {
    Clock clock;
    float minFps = 30;
    SDL_Event event;
    float msPerFrame = 1000.f / minFps;
    uint32_t msElapsed = 0;
    while (!m_window.shouldClose()) {
        while (m_window.pollEvent(event)) {
            onEventPoll(event);
        }
        onEventProcess();

        msElapsed = clock.restart();
        while (msElapsed > msPerFrame) {
            msElapsed -= msPerFrame;
            tick(msPerFrame / 1000.f);
        }
        tick(msElapsed / 1000.f);

        render();
    }
}

void Application::quit() { m_window.setShouldClose(true); }

void Application::tick(float dt) {
    InputManager::instance().tick();

    for (auto &layer : m_layers) {
        layer->onTick(dt);
    }
}

void Application::render() {
    for (auto &layer : m_layers) {
        layer->onRender();
    }
    m_imguiLayer->begin();
    for (auto &layer : m_layers) {
        layer->onImGui();
    }
    m_imguiLayer->end();

    m_window.swapBuffer();
}

}  // namespace sd
