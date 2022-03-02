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

static Window *s_window = nullptr;
void Window::Init(const WindowProp &property) {
    SD_CORE_TRACE("Initializing Window...");
    switch (GetGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            s_window = new GLWindow(property);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
}

void Window::Shutdown() {
    delete s_window;
    s_window = nullptr;
}

Window &Window::Get() { return *s_window; }

bool Window::PollEvent(ApplicationEvent &event) {
    SDL_Event sdl_event;
    if (SDL_PollEvent(&sdl_event) != 1) return false;
    switch (sdl_event.type) {
        case SDL_MOUSEMOTION: {
            event.type = EventType::MOUSE_MOTION;
            event.mouse_motion.x = sdl_event.motion.x;
            event.mouse_motion.y = sdl_event.motion.y;
            event.mouse_motion.x_rel = sdl_event.motion.xrel;
            event.mouse_motion.y_rel = sdl_event.motion.yrel;
            Input::SetMouseCoord(event.mouse_motion.x, event.mouse_motion.y);
        } break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP: {
            event.type = EventType::MOUSE_BUTTON;
            event.mouse_button.button =
                static_cast<MouseButton>(sdl_event.button.button);
            event.mouse_button.x = sdl_event.button.x;
            event.mouse_button.y = sdl_event.button.y;
            event.mouse_button.clicks = sdl_event.button.clicks;
            event.mouse_button.state = sdl_event.button.state;
            Input::SetMouseButtonState(event.mouse_button.button,
                                       event.mouse_button.state);
        } break;
        case SDL_MOUSEWHEEL: {
            event.type = EventType::MOUSE_WHEEL_SCROLLED;
            event.mouse_wheel.x = sdl_event.wheel.x;
            event.mouse_wheel.y = sdl_event.wheel.y;
        } break;
        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            event.type = EventType::KEY;
            event.key.keycode = static_cast<Keycode>(sdl_event.key.keysym.sym);
            event.key.mod = sdl_event.key.keysym.mod;
            event.key.state = sdl_event.key.state;
            Input::SetKeyState(event.key.keycode, event.key.state);
        } break;
        case SDL_WINDOWEVENT: {
            switch (sdl_event.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_SIZE_CHANGED: {
                    event.type = EventType::WINDOW_RESIZED;
                    event.window_size.width = sdl_event.window.data1;
                    event.window_size.height = sdl_event.window.data2;
                } break;
            }
        } break;
        case SDL_TEXTINPUT: {
            event.type = EventType::TEXT_INPUT;
            std::copy(std::begin(sdl_event.text.text),
                      std::end(sdl_event.text.text), event.text_input.text);
        } break;
        case SDL_QUIT: {
            event.type = EventType::APP_QUIT;
        } break;
    }
    return true;
}
bool Window::ShouldClose() { return m_should_close; }

void Window::SetShouldClose(bool shouldClose) { m_should_close = shouldClose; }

}  // namespace SD
