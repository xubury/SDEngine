#include "Core/Application.hpp"
#include "Utils/Log.hpp"
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

    sd::Renderer::getDefaultTarget().resize(width, height);
    m_imguiLayer = createRef<ImGuiLayer>();
    pushOverlay(m_imguiLayer);
}

Application::~Application() {
    IMG_Quit();
    SDL_Quit();
}

void Application::pushLayer(Ref<Layer> layer) { m_layers.pushLayer(layer); }

void Application::pushOverlay(Ref<Layer> layer) { m_layers.pushOverlay(layer); }

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
    FpsLimiter fpsLimiter;
    fpsLimiter.init(60);
    SDL_Event event;
    uint32_t lastTicks = SDL_GetTicks();
    while (!m_window.shouldClose()) {
        fpsLimiter.begin();

        while (m_window.pollEvent(event)) {
            onEventPoll(event);
        }
        onEventProcess();

        uint32_t newTicks = SDL_GetTicks();
        tick((newTicks - lastTicks) / 1000.f);
        lastTicks = newTicks;

        render();

        fpsLimiter.end();
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
    bool imgui = m_layers.hasLayer(m_imguiLayer);
    if (imgui) {
        m_imguiLayer->begin();
        for (auto &layer : m_layers) {
            layer->onImGui();
        }
        m_imguiLayer->end();
    }

    m_window.swapBuffer();
}

}  // namespace sd
