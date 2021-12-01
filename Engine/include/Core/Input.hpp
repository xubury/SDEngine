#ifndef SD_INPUT_ENGINE_HPP
#define SD_INPUT_ENGINE_HPP

#include "Utility/Base.hpp"
#include "Core/Event.hpp"

#include <glm/glm.hpp>

namespace SD {

class SD_CORE_API Input {
   public:
    static bool IsKeyDown(Keycode keycode);
    static bool WasKeyDown(Keycode keycode);
    static bool IsKeyPressed(Keycode keycode);

    static bool IsMouseDown(MouseButton mouse);
    static bool WasMouseDown(MouseButton mouse);
    static bool IsMousePressed(MouseButton mouse);

    static glm::vec2 GetMouseCoord();

   private:
    friend class Application;

    static void Tick();

    static void PressKey(Keycode keycode);
    static void ReleaseKey(Keycode keycode);

    static void PressMouseButton(MouseButton mouse);
    static void ReleaseMouseButton(MouseButton mouse);

    static void SetMouseCoord(float x, float y);
};

}  // namespace SD

#endif /* SD_INPUT_ENGINE_HPP */
