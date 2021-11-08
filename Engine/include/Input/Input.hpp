#ifndef SD_INPUT_ENGINE_HPP
#define SD_INPUT_ENGINE_HPP

#include <glm/glm.hpp>
#include "Utility/Export.hpp"
#include <SDL.h>

namespace sd {

class SD_API Input {
   public:
    static bool isKeyDown(SDL_Keycode keycode);
    static bool wasKeyDown(SDL_Keycode keycode);
    static bool isKeyPressed(SDL_Keycode keycode);

    static bool isMouseDown(uint8_t button);
    static bool wasMouseDown(uint8_t button);
    static bool isMousePressed(uint8_t button);

    static glm::vec2 getMouseCoord();

   private:
    friend class Application;

    static void tick();

    static void processEvent(const SDL_Event &event);
    static void pressKey(SDL_Keycode keycode);
    static void releaseKey(SDL_Keycode keycode);

    static void pressMouseButton(uint8_t button);
    static void releaseMouseButton(uint8_t button);

    static void setMouseCoord(float x, float y);
};

}  // namespace sd

#endif /* SD_INPUT_ENGINE_HPP */
