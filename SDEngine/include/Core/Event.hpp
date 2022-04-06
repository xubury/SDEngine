#ifndef SD_EVENT_HPP
#define SD_EVENT_HPP

#include "Utility/Base.hpp"
#include "Core/Export.hpp"
#include "Core/Keyboard.hpp"
#include "Core/Mouse.hpp"
#include "ECS/Entity.hpp"

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

struct AppQuitEvent {
};

}  // namespace SD

#endif /* SD_EVENT_HPP */
