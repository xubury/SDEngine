#ifndef SD_INPUT_ENGINE_HPP
#define SD_INPUT_ENGINE_HPP

#include "Utility/Base.hpp"
#include "Core/Keyboard.hpp"
#include "Core/Mouse.hpp"
#include "Core/Export.hpp"
#include "Utility/EventDispatcher.hpp"

#include <unordered_map>
#include <glm/glm.hpp>

namespace SD {

class SD_CORE_API Input {
   public:
    static void Init(EventDispatcher *dispatcher);
    static void Shutdown();
    static bool IsKeyDown(Keycode keycode);
    static bool WasKeyDown(Keycode keycode);
    static bool IsKeyPressed(Keycode keycode);

    static bool IsMouseDown(MouseButton mouse);
    static bool WasMouseDown(MouseButton mouse);
    static bool IsMousePressed(MouseButton mouse);

    static glm::vec2 GetMouseCoord();

    static void Tick();

   private:
    static std::unordered_map<Keycode, bool> s_key;
    static std::unordered_map<Keycode, bool> s_last_key;

    static std::unordered_map<MouseButton, bool> s_mouse_button;
    static std::unordered_map<MouseButton, bool> s_last_mouse_button;

    static glm::vec2 s_mouse_coord;

    static std::vector<HandlerRegistration> s_handlers;

    static void SetKeyState(Keycode keycode, bool state);
    static void SetMouseButtonState(MouseButton mouse, bool state);
    static void SetMouseCoord(float x, float y);
};

}  // namespace SD

#endif /* SD_INPUT_ENGINE_HPP */
