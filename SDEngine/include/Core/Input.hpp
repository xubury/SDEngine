#ifndef SD_INPUT_ENGINE_HPP
#define SD_INPUT_ENGINE_HPP

#include "Utility/Base.hpp"
#include "Utility/Math.hpp"
#include "Core/Keyboard.hpp"
#include "Core/Mouse.hpp"
#include "Core/Export.hpp"
#include "Core/EventDispatcher.hpp"

#include <unordered_map>

namespace SD {

class SD_CORE_API Input {
   public:
    static bool IsKeyDown(Keycode keycode);
    static bool WasKeyDown(Keycode keycode);
    static bool IsKeyPressed(Keycode keycode);

    static bool IsMouseDown(MouseButton mouse);
    static bool WasMouseDown(MouseButton mouse);
    static bool IsMousePressed(MouseButton mouse);

    static Vector2f GetMouseCoord();

    static void Tick();

    static void SetKeyState(Keycode keycode, bool state);
    static void SetMouseButtonState(MouseButton mouse, bool state);
    static void SetMouseCoord(float x, float y);

   private:
    static std::unordered_map<Keycode, bool> s_key;
    static std::unordered_map<Keycode, bool> s_last_key;

    static std::unordered_map<MouseButton, bool> s_mouse_button;
    static std::unordered_map<MouseButton, bool> s_last_mouse_button;

    static Vector2f s_mouse_coord;
};

}  // namespace SD

#endif /* SD_INPUT_ENGINE_HPP */
