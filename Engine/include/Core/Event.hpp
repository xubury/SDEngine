#ifndef SD_EVENT_HPP
#define SD_EVENT_HPP

#include "Utility/Base.hpp"
#include "Core/Keycode.hpp"
#include "Core/Mouse.hpp"

namespace SD {

struct WindowSizeEvent {
    uint32_t width;
    uint32_t height;
};

struct KeyEvent {
    Keycode keycode;
    uint16_t mod;
};

struct MouseMotionEvent {
    int32_t x;
    int32_t y;
    int32_t xrel;
    int32_t yrel;
};

struct MouseButtonEvent {
    MouseButton button;
    int32_t x;
    int32_t y;
    uint8_t clicks;
};

struct MouseWheelEvent {
    int32_t x;
    int32_t y;
};

struct SD_API Event {
    union {
        WindowSizeEvent size;
        KeyEvent key;
        MouseMotionEvent mouseMotion;
        MouseButtonEvent mouseButton;
        MouseWheelEvent mouseWheel;
    };

    enum EventType {
        UNKNOWN = 0,
        WINDOW_RESIZED,
        KEY_PRESSED,
        KEY_RELEASED,
        MOUSE_MOTION,
        MOUSE_BUTTON_PRESSED,
        MOUSE_BUTTON_RELEASED,
        MOUSE_WHEEL_SCROLLED
    };

    EventType type = Event::UNKNOWN;
    bool handled = false;
};

}  // namespace SD

#endif /* SD_EVENT_HPP */
