#include "Core/Window.hpp"
#include "Core/Context.hpp"
#include "Utility/Log.hpp"
#include "Graphics/Graphics.hpp"

namespace sd {

Window::Window() : m_shouldClose(false) {}

bool Window::create(const WindowProp &property) {
    SD_CORE_TRACE("Initializing window...");

    m_context = Context::create(property, &m_window);
    if (m_window == nullptr) {
        SD_CORE_ERROR("SDL_CreateWindow failed: {}", SDL_GetError());
        return false;
    }

    return true;
}

Window::~Window() { SDL_DestroyWindow(m_window); }

bool Window::pollEvent(SDL_Event &event) { return SDL_PollEvent(&event) == 1; }

bool Window::shouldClose() const { return m_shouldClose; }

void Window::setShouldClose(bool shouldClose) { m_shouldClose = shouldClose; }

void Window::swapBuffer() { m_context->swapBuffer(); }

int Window::getWidth() const {
    int w = 0, _;
    SDL_GetWindowSize(m_window, &w, &_);
    return w;
}

int Window::getHeight() const {
    int h = 0, _;
    SDL_GetWindowSize(m_window, &_, &h);
    return h;
}

SDL_Window *Window::getHandle() const { return m_window; }

void *Window::getGraphicContext() const { return m_context->getHandle(); }

}  // namespace sd
