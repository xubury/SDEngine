#ifndef SD_INPUT_MANAGER_HPP
#define SD_INPUT_MANAGER_HPP

#include <SDL.h>
#include <unordered_map>
#include <glm/glm.hpp>
#include "Common/Export.hpp"

namespace sd {

class SD_API InputManager {
   public:
    static bool isKeyDown(SDL_Keycode keycode);
    static bool wasKeyDown(SDL_Keycode keycode);
    static bool isKeyPressed(SDL_Keycode keycode);

    static bool isMouseDown(uint8_t button);
    static bool wasMouseDown(uint8_t button);
    static bool isMousePressed(uint8_t button);

    static glm::vec2 getMouseCoord();

   private:
    static InputManager &instance();

    friend class Application;

    void tick();

    void pressKey(SDL_Keycode keycode);
    void releaseKey(SDL_Keycode keycode);

    void pressMouseButton(uint8_t button);
    void releaseMouseButton(uint8_t button);

    void setMouseCoord(float x, float y);

    InputManager() = default;
    ~InputManager() = default;

    InputManager(const InputManager &) = delete;
    InputManager &operator=(const InputManager &) = delete;

    std::unordered_map<SDL_Keycode, bool> m_keyMap;
    std::unordered_map<SDL_Keycode, bool> m_lastKeyMap;

    std::unordered_map<uint8_t, bool> m_mouseBtnMap;
    std::unordered_map<uint8_t, bool> m_lastMouseBtnMap;

    glm::vec2 m_mouseCoord;
};

}  // namespace sd

#endif /* SD_INPUT_MANAGER */
