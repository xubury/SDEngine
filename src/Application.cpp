#include "Application.hpp"
#include "Core/Log.hpp"

namespace sd {

Application::Application() : m_isInit(false) {}

Application::~Application() {}

bool Application::init() {

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

void Application::run() {
    if (!m_isInit) {
        return;
    }

    while (!m_window.shouldClose()) {
        SDL_Event event;
        while (m_window.pollEvent(event)) {
            if (event.type == SDL_QUIT) {
                m_window.setShouldClose(true);
            }
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                m_window.setShouldClose(true);
            }
        }
    }
}

void Application::free() {
    m_window.free();
    SDL_Quit();
}

}  // namespace sd
