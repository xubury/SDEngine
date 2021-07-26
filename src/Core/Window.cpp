#include "Core/Window.hpp"

namespace sd {

Window::Window() : m_shouldClose(false) {}

bool Window::init() {
    m_window =
        SDL_CreateWindow("SDL Window", 50, 50, 800, 600, SDL_WINDOW_SHOWN);
    return m_window != nullptr;
}

void Window::free() { SDL_DestroyWindow(m_window); }

Window::~Window() { free(); }

bool Window::pollEvent(SDL_Event &event) { return SDL_PollEvent(&event) == 1; }

bool Window::shouldClose() const { return m_shouldClose; }

void Window::setShouldClose(bool shouldClose) { m_shouldClose = shouldClose; }

}  // namespace S2D
