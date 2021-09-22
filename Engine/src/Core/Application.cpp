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

Window &Application::getWindow() { return s_instance->m_window; }

RenderEngine &Application::getRenderEngine() {
    return s_instance->m_renderEngine;
}

Application::Application() {
    std::string debugPath = "Debug.txt";
    Log::init(debugPath);
    int width = 1280;
    int height = 720;
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

    WindowProp prop;
    prop.width = width;
    prop.height = height;
    if (!m_window.create(prop)) {
        exit(-1);
    }

    s_instance = this;

    sd::Renderer::init();

    sd::Renderer2D::init();
    sd::Renderer3D::init();

    sd::Renderer::getDefaultTarget().resize(width, height);
    m_imguiLayer = new ImGuiLayer();
    pushOverlay(m_imguiLayer);

    m_renderEngine.init();
}

Application::~Application() {
    IMG_Quit();
    SDL_Quit();
}

void Application::pushLayer(Layer *layer) { m_layers.pushLayer(layer); }

void Application::pushOverlay(Layer *layer) { m_layers.pushOverlay(layer); }

void Application::popLayer(Layer *layer) { m_layers.popLayer(layer); }

void Application::popOverlay(Layer *layer) { m_layers.popOverlay(layer); }

void Application::processEvent(const SDL_Event &event) {
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
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                Renderer::getDefaultTarget().resize(event.window.data1,
                                                    event.window.data2);
                m_renderEngine.resize(event.window.data1, event.window.data2);
            }
            break;
        default:
            break;
    }
    for (auto layer = m_layers.rbegin(); layer != m_layers.rend(); ++layer) {
        (*layer)->onEventProcess(event);
        if ((*layer)->isBlockEvent()) break;
    }
}

void Application::processEvents() {
    for (auto layer = m_layers.rbegin(); layer != m_layers.rend(); ++layer) {
        (*layer)->onEventsProcess();
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
            processEvent(event);
        }
        processEvents();

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

    m_renderEngine.tick(dt);
    for (auto &layer : m_layers) {
        layer->onTick(dt);
    }
}

void Application::render() {
    m_renderEngine.render();

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
