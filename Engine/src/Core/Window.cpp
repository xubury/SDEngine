#include "Core/Window.hpp"
#include "Utils/Log.hpp"
#include "Graphics/Graphics.hpp"

namespace sd {

Window::Window() : m_shouldClose(false) {}

bool Window::create(const std::string &title, int width, int height,
                    WindowFlag flags) {
    SD_CORE_TRACE("Initializing window...");
    uint32_t sdlFlags = SDL_WINDOW_RESIZABLE;
    switch (Graphics::getAPI()) {
        case API::OpenGL:
            sdlFlags |= SDL_WINDOW_OPENGL;
            break;
        default:
            break;
    }
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
