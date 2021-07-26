#include "Application.hpp"

namespace sd {

Application::Application() : m_isInit(false) {}

Application::~Application() { SDL_Quit(); }

bool Application::init() {
    m_isInit = true;
    do {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            m_isInit = false;
            break;
        }
        if (!m_window.init()) {
            m_isInit = false;
            break;
        }
    } while (false);

    return m_isInit;
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

void Application::free() { m_window.free(); }

}  // namespace S2D
