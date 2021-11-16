#include "Core/Window.hpp"
#include "Core/SDL.hpp"
#include "Core/OpenGL/GLWindow.hpp"
#include "Graphics/Graphics.hpp"

namespace SD {

Ref<Window> Window::create(const WindowProp &property) {
    SD_CORE_TRACE("Initializing Window...");
    Ref<Window> window;
    switch (getGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            window = createRef<GLWindow>(property);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return window;
}

Window::Window(const WindowProp &property) : m_samples(property.samples) {}

Window::~Window() { SDL_DestroyWindow(m_window); }

bool Window::pollEvent(SDL_Event &event) { return SDL_PollEvent(&event) == 1; }

bool Window::shouldClose() { return m_shouldClose; }

void Window::setShouldClose(bool shouldClose) { m_shouldClose = shouldClose; }

glm::ivec2 Window::getSize() {
    glm::ivec2 size;
    SDL_GetWindowSize(m_window, &size.x, &size.y);
    return size;
}

SDL_Window *Window::getHandle() { return m_window; }

void Window::swapBuffer() { SDL_GL_SwapWindow(m_window); }

}  // namespace SD
