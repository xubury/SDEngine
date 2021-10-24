#include "Core/Window.hpp"
#include "Core/OpenGL/GLWindow.hpp"
#include "Graphics/Graphics.hpp"
#include "Utility/Log.hpp"

namespace sd {

static Window *s_window = nullptr;

void Window::init(const WindowProp &property) {
    SD_CORE_TRACE("Initializing Window...");
    switch (Graphics::getAPI()) {
        case API::OpenGL:
            s_window = new GLWindow(property);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
}

bool Window::pollEvent(SDL_Event &event) { return SDL_PollEvent(&event) == 1; }

bool Window::shouldClose() { return s_window->m_shouldClose; }

void Window::setShouldClose(bool shouldClose) {
    s_window->m_shouldClose = shouldClose;
}

int Window::getWidth() {
    int w = 0, _;
    SDL_GetWindowSize(s_window->m_window, &w, &_);
    return w;
}

int Window::getHeight() {
    int h = 0, _;
    SDL_GetWindowSize(s_window->m_window, &_, &h);
    return h;
}

SDL_Window *Window::getHandle() { return s_window->m_window; }

void Window::swapBuffer() { SDL_GL_SwapWindow(s_window->m_window); }

void *Window::getGraphicsContext() { return s_window->m_context; }

}  // namespace sd
