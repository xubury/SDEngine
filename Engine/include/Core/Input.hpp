#ifndef SD_INPUT_HPP
#define SD_INPUT_HPP

#include <unordered_map>
#include <glm/glm.hpp>
#include "Core/SDL.hpp"
#include "Utility/Export.hpp"

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
};

}  // namespace sd

#endif /* SD_INPUT_HPP */
