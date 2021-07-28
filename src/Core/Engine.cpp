#include "Core/Engine.hpp"
#include "Core/Log.hpp"
#include "Core/InputManager.hpp"
#include "Core/Timing.hpp"
#include <GL/gl.h>

namespace sd {

Engine::Engine() : m_isInit(false) {}

Engine::~Engine() {}

bool Engine::create() {
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

void Engine::processEvent(const SDL_Event &event) {
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

void Engine::run() {
    if (!m_isInit) {
        return;
    }

    FpsLimiter fpsLimiter;
    fpsLimiter.init(60);
    SDL_Event event;
    while (!m_window.shouldClose()) {
        fpsLimiter.begin();

        while (m_window.pollEvent(event)) {
            processEvent(event);
        }
        tick();
        render();

        fpsLimiter.end();
    }
}

void Engine::tick() { InputManager::instance().tick(); }

void Engine::render() {
    glClearColor(0.1, 0.2, 0.3, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    m_window.swapBuffer();
}

void Engine::destroy() {
    m_window.destroy();
    SDL_Quit();
}

}  // namespace sd
