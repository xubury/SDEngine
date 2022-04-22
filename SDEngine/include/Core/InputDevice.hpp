#ifndef SD_INPUT_ENGINE_HPP
#define SD_INPUT_ENGINE_HPP

#include "Utility/Base.hpp"
#include "Utility/Math.hpp"
#include "Core/Keyboard.hpp"
#include "Core/Mouse.hpp"
#include "Core/Export.hpp"

#include <unordered_map>

namespace SD {

class SD_CORE_API InputDevice {
   public:
    bool IsKeyDown(Keycode keycode);
    bool WasKeyDown(Keycode keycode);
    bool IsKeyPressed(Keycode keycode);

    bool IsMouseDown(MouseButton mouse);
    bool WasMouseDown(MouseButton mouse);
    bool IsMousePressed(MouseButton mouse);

    Vector2f GetMouseCoord();
    Vector2f GetMouseWheel();

    void Tick();

    void SetKeyState(Keycode keycode, bool state);
    void SetMouseButtonState(MouseButton mouse, bool state);
    void SetMouseCoord(float x, float y);
    void SetMouseWheel(float x, float y);

   private:
    std::unordered_map<Keycode, bool> m_key;
    std::unordered_map<Keycode, bool> m_last_key;

    std::unordered_map<MouseButton, bool> m_mouse_button;
    std::unordered_map<MouseButton, bool> m_last_mouse_button;

    Vector2f m_mouse_wheel;
    Vector2f m_mouse_coord;
};

}  // namespace SD

#endif /* SD_INPUT_ENGINE_HPP */
