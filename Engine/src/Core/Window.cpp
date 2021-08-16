#include "Core/Window.hpp"
#include "Common/Log.hpp"
#include "Graphics/Graphics.hpp"

namespace sd {

Window::Window() : m_shouldClose(false) {}

bool Window::create(const WindowProp &property) {
    SD_CORE_TRACE("Initializing window...");
    uint32_t sdlFlags = SDL_WINDOW_RESIZABLE;
    switch (Graphics::getAPI()) {
        case API::OpenGL:
            sdlFlags |= SDL_WINDOW_OPENGL;
            break;
        default:
            break;
    }
    if (property.flag & INVISIBLE) {
        sdlFlags |= SDL_WINDOW_HIDDEN;
    }
    if (property.flag & FULLSCREEN) {
        sdlFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    if (property.flag & BORDERLESS) {
        sdlFlags |= SDL_WINDOW_BORDERLESS;
    }

    m_window = SDL_CreateWindow(property.title.c_str(), property.x, property.y,
                                property.width, property.height, sdlFlags);
    if (m_window == nullptr) {
        SD_CORE_ERROR("SDL_CreateWindow failed: {}", SDL_GetError());
        return false;
    }

    m_context = Context::create(m_window);

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
