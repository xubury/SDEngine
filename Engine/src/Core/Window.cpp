#include "Core/Window.hpp"
#include "Core/OpenGL/GLWindow.hpp"
#include "Graphics/Graphics.hpp"
#include "Utility/Log.hpp"

namespace sd {

Ref<Window> Window::create(const WindowProp &property) {
    SD_CORE_TRACE("Initializing Window...");
    Ref<Window> context;
    switch (Graphics::getAPI()) {
        case API::OpenGL:
            context = createRef<GLWindow>(property);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return context;
}

bool Window::pollEvent(SDL_Event &event) { return SDL_PollEvent(&event) == 1; }

bool Window::shouldClose() const { return m_shouldClose; }

void Window::setShouldClose(bool shouldClose) { m_shouldClose = shouldClose; }

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

void Window::swapBuffer() const { SDL_GL_SwapWindow(m_window); }

}  // namespace sd
