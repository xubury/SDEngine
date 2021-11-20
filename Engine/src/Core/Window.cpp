#include "Core/Window.hpp"
#include "Core/SDL.hpp"
#include "Core/OpenGL/GLWindow.hpp"
#include "Graphics/Graphics.hpp"
#include "Renderer/RenderTarget.hpp"

namespace SD {

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

glm::ivec2 Window::GetSize() {
    glm::ivec2 size;
    SDL_GetWindowSize(m_window, &size.x, &size.y);
    return size;
}

SDL_Window *Window::GetHandle() { return m_window; }

std::string Window::GetTitle() const { return SDL_GetWindowTitle(m_window); }

void Window::SwapBuffer() { SDL_GL_SwapWindow(m_window); }

glm::vec2 Window::MapScreenToClip(const RenderTarget &target,
                                  const glm::ivec2 &pos) const {
    glm::vec2 clip;
    int h = 0;
    SDL_GetWindowSize(m_window, nullptr, &h);
    clip.x = 2.f * (pos.x - target.GetX()) / target.GetWidth() - 1.f;
    clip.y = 2.f * (h - pos.y - target.GetY()) / target.GetHeight() - 1.f;
    return clip;
}

glm::ivec2 Window::MapClipToScreen(const RenderTarget &target,
                                   const glm::vec2 &pos) const {
    glm::ivec2 screen;
    int h = 0;
    SDL_GetWindowSize(m_window, nullptr, &h);
    screen.x = (pos.x + 1.f) * target.GetWidth() / 2.f + target.GetX();
    screen.y = (pos.y + 1.f) * target.GetHeight() / 2.f + target.GetY();
    screen.y = h - screen.y;
    return screen;
}

}  // namespace SD
