#include "Core/Window.hpp"
#include "Core/SDL.hpp"
#include "Core/OpenGL/GLWindow.hpp"
#include "Graphics/Graphics.hpp"

namespace SD {

WindowProp::WindowProp()
    : title("SD Engine"),
      x(SDL_WINDOWPOS_CENTERED),
      y(SDL_WINDOWPOS_CENTERED),
      width(1440),
      height(900),
      msaa(8),
      vsync(true),
      flag(0) {}

Ref<Window> Window::Create(const WindowProp &property) {
    SD_CORE_TRACE("Initializing Window...");
    Ref<Window> window;
    switch (GetGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            window = CreateRef<GLWindow>(property);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return window;
}

Window::~Window() { SDL_DestroyWindow(m_window); }

bool Window::PollEvent(SDL_Event &event) { return SDL_PollEvent(&event) == 1; }

bool Window::ShouldClose() { return m_shouldClose; }

void Window::SetShouldClose(bool shouldClose) { m_shouldClose = shouldClose; }

glm::ivec2 Window::GetSize() const {
    glm::ivec2 size;
    SDL_GetWindowSize(m_window, &size.x, &size.y);
    return size;
}

int Window::GetWidth() const {
    int width = 0;
    SDL_GetWindowSize(m_window, &width, nullptr);
    return width;
}

int Window::GetHeight() const {
    int height = 0;
    SDL_GetWindowSize(m_window, nullptr, &height);
    return height;
}

SDL_Window *Window::GetHandle() { return m_window; }

std::string Window::GetTitle() const { return SDL_GetWindowTitle(m_window); }

void Window::SwapBuffer() { SDL_GL_SwapWindow(m_window); }

}  // namespace SD
