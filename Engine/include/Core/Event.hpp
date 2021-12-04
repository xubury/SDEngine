#ifndef SD_EVENT_HPP
#define SD_EVENT_HPP

#include "Utility/Base.hpp"
#include "Core/Export.hpp"
#include "Core/Keyboard.hpp"
#include "Core/Mouse.hpp"

namespace SD {

struct WindowSizeEvent {
    uint32_t width;
    uint32_t height;
};

struct KeyEvent {
    Keycode keycode;
    uint16_t mod;
    bool state;
};

struct MouseMotionEvent {
    int32_t x;
    int32_t y;
    int32_t x_rel;
    int32_t y_rel;
};

struct MouseButtonEvent {
    MouseButton button;
    int32_t x;
    int32_t y;
    uint8_t clicks;
    bool state;
};

struct MouseWheelEvent {
    int32_t x;
    int32_t y;
};

struct TextInputEvent {
    char text[32];  // std::string is not allowed in union
};

enum class EventType {
    UNKNOWN = 0,
    WINDOW_RESIZED,
    KEY,
    MOUSE_MOTION,
    MOUSE_BUTTON,
    MOUSE_WHEEL_SCROLLED,
    TEXT_INPUT
};

struct SD_CORE_API Event {
    union {
        WindowSizeEvent window_size;
        KeyEvent key;
        MouseMotionEvent mouse_motion;
        MouseButtonEvent mouse_button;
        MouseWheelEvent mouse_wheel;
        TextInputEvent text_input;
    };

    EventType type = EventType::UNKNOWN;
    bool handled = false;
};

}  // namespace SD

#endif /* SD_EVENT_HPP */
