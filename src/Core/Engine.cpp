#include "Core/Engine.hpp"
#include "Core/Log.hpp"

namespace sd {

Engine::Engine() : m_isInit(false) {}

Engine::~Engine() {}

bool Engine::init() {
    std::string debugPath = "Debug.txt";
    Log::init(debugPath);
    SD_CORE_INFO("Debug info is output to: {}", debugPath);

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SD_CORE_ERROR("SDL2 init failed: {}", SDL_GetError());
        return false;
    }
    if (!m_window.init()) {
        SD_CORE_ERROR("Window init failed: {}", SDL_GetError());
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
        default:
            break;
    }
}

void Engine::run() {
    if (!m_isInit) {
        return;
    }
    while (!m_window.shouldClose()) {
        SDL_Event event;
        while (m_window.pollEvent(event)) {
            processEvent(event);
        }
    }
}

void Engine::free() {
    m_window.free();
    SDL_Quit();
}

}  // namespace sd
