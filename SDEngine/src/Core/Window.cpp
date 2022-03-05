#include "Core/Window.hpp"
#include "Core/SDL.hpp"
#include "Core/Input.hpp"
#include "Core/OpenGL/GLWindow.hpp"
#include "Graphics/Graphics.hpp"

namespace SD {

WindowProp::WindowProp(const std::string &title, int32_t x, int32_t y,
                       int32_t width, int32_t height, int32_t msaa, bool vsync,
                       uint32_t flag)
    : title(title),
      x(x),
      y(y),
      width(width),
      height(height),
      msaa(msaa),
      vsync(vsync),
      flag(flag) {}

Scope<Window> Window::Create(const WindowProp &property) {
    SD_CORE_TRACE("Initializing Window...");
    Scope<Window> window;
    switch (GetGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            window = CreateScope<GLWindow>(property);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return window;
}

void Window::PollEvents() {
    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event) == 1) {
        switch (sdl_event.type) {
            case SDL_MOUSEMOTION: {
                MouseMotionEvent event;
                event.x = sdl_event.motion.x;
                event.y = sdl_event.motion.y;
                event.x_rel = sdl_event.motion.xrel;
                event.y_rel = sdl_event.motion.yrel;
                Input::SetMouseCoord(event.x, event.y);
                EventSystem::Get().PublishEvent(event);
            } break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP: {
                MouseButtonEvent event;
                event.button =
                    static_cast<MouseButton>(sdl_event.button.button);
                event.x = sdl_event.button.x;
                event.y = sdl_event.button.y;
                event.clicks = sdl_event.button.clicks;
                event.state = sdl_event.button.state;
                Input::SetMouseButtonState(event.button, event.state);
                EventSystem::Get().PublishEvent(event);
            } break;
            case SDL_MOUSEWHEEL: {
                MouseWheelEvent event;
                event.x = sdl_event.wheel.x;
                event.y = sdl_event.wheel.y;
                EventSystem::Get().PublishEvent(event);
            } break;
            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                KeyEvent event;
                event.keycode = static_cast<Keycode>(sdl_event.key.keysym.sym);
                event.mod = sdl_event.key.keysym.mod;
                event.state = sdl_event.key.state;
                Input::SetKeyState(event.keycode, event.state);
                EventSystem::Get().PublishEvent(event);
            } break;
            case SDL_WINDOWEVENT: {
                switch (sdl_event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED: {
                        WindowSizeEvent event;
                        event.width = sdl_event.window.data1;
                        event.height = sdl_event.window.data2;
                        EventSystem::Get().PublishEvent(event);
                    } break;
                }
            } break;
            case SDL_TEXTINPUT: {
                TextInputEvent event;
                std::copy(std::begin(sdl_event.text.text),
                          std::end(sdl_event.text.text), event.text);
                EventSystem::Get().PublishEvent(event);
            } break;
            case SDL_QUIT: {
                AppQuitEvent event;
                EventSystem::Get().PublishEvent(event);
            } break;
        }
    }
}

bool Window::ShouldClose() { return m_should_close; }

void Window::SetShouldClose(bool shouldClose) { m_should_close = shouldClose; }

}  // namespace SD
