#ifndef SD_INPUT_ENGINE_HPP
#define SD_INPUT_ENGINE_HPP

#include "Utility/Base.hpp"
#include "Core/Keycode.hpp"
#include "Core/Event.hpp"

#include <glm/glm.hpp>

namespace SD {

class SD_API Input {
   public:
    static bool isKeyDown(Keycode keycode);
    static bool wasKeyDown(Keycode keycode);
    static bool isKeyPressed(Keycode keycode);

    static bool isMouseDown(MouseButton mouse);
    static bool wasMouseDown(MouseButton mouse);
    static bool isMousePressed(MouseButton mouse);

    static glm::vec2 getMouseCoord();

   private:
    friend class Application;

    static void tick();

    static void processEvent(const Event &event);
    static void pressKey(Keycode keycode);
    static void releaseKey(Keycode keycode);

    static void pressMouseButton(MouseButton mouse);
    static void releaseMouseButton(MouseButton mouse);

    static void setMouseCoord(float x, float y);
};

}  // namespace SD

#endif /* SD_INPUT_ENGINE_HPP */
