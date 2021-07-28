#include "Core/Application.hpp"
#include "Core/Log.hpp"
#include "Core/InputManager.hpp"
#include "Core/Timing.hpp"
#include <GL/gl.h>

namespace sd {

Application *Application::s_instance = nullptr;

Application &Application::instance() { return *s_instance; }

Application::Application() : m_isInit(false) { s_instance = this; }

Application::~Application() {}

bool Application::init() {
    std::string debugPath = "Debug.txt";
    Log::init(debugPath);
    SD_CORE_INFO("Debug info is output to: {}", debugPath);

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SD_CORE_ERROR("SDL_Init failed: {}", SDL_GetError());
        return false;
    }
    if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) != 0) {
        SD_CORE_ERROR("SDL_GL_SetAttribute Failed: {}", SDL_GetError());
        return false;
    }
    if (!m_window.create("SD Engine", 800, 600, Window::WINDOWED)) {
        return false;
    }
    m_isInit = true;
    return true;
}

void Application::onEventPoll(const SDL_Event &event) {
    switch (event.type) {
        case SDL_QUIT:
            m_window.setShouldClose(true);
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
        default:
            break;
    }
}

void Application::run() {
    if (!m_isInit) {
        return;
    }

    FpsLimiter fpsLimiter;
    fpsLimiter.init(60);
    SDL_Event event;
    uint32_t lastTicks = SDL_GetTicks();
    while (!m_window.shouldClose()) {
        fpsLimiter.begin();

        while (m_window.pollEvent(event)) {
            onEventPoll(event);
            for (auto &layer : m_layers) {
                layer->onEventPoll(event);
            }
        }
        for (auto &layer : m_layers) {
            layer->onEventProcess();
        }

        uint32_t newTicks = SDL_GetTicks();
        tick(newTicks - lastTicks);
        lastTicks = newTicks;

        render();

        fpsLimiter.end();
    }
}

void Application::tick(float dt) {
    InputManager::instance().tick();

    for (auto &layer : m_layers) {
        layer->tick(dt);
    }
}

void Application::render() {
    glClearColor(0.1, 0.2, 0.3, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    for (auto &layer : m_layers) {
        layer->render();
    }

    for (auto &layer : m_layers) {
        layer->renderImGui();
    }

    m_window.swapBuffer();
}

void Application::destroy() {
    m_window.destroy();
    SDL_Quit();
}

}  // namespace sd
