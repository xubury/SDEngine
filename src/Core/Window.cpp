#include "Core/Window.hpp"

namespace sd {

Window::Window() : m_shouldClose(false) {}

bool Window::create(const std::string &title, int width, int height,
                    WindowFlag flags) {
    uint32_t sdlFlags = SDL_WINDOW_OPENGL;
    if (flags & INVISIBLE) {
        sdlFlags |= SDL_WINDOW_HIDDEN;
    }
    if (flags & FULLSCREEN) {
        sdlFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    if (flags & BORDERLESS) {
        sdlFlags |= SDL_WINDOW_BORDERLESS;
    }
    m_window =
        SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, width, height, sdlFlags);
    return m_window != nullptr;
}

void Window::destroy() { SDL_DestroyWindow(m_window); }

Window::~Window() { destroy(); }

bool Window::pollEvent(SDL_Event &event) { return SDL_PollEvent(&event) == 1; }

bool Window::shouldClose() const { return m_shouldClose; }

void Window::setShouldClose(bool shouldClose) { m_shouldClose = shouldClose; }

}  // namespace sd
