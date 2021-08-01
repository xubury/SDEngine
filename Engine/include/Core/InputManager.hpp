#ifndef SD_INPUT_MANAGER_HPP
#define SD_INPUT_MANAGER_HPP

#include <SDL.h>
#include <unordered_map>
#include "Utils/Export.hpp"

namespace sd {

class SD_API InputManager {
   public:
    static InputManager &instance();

    bool isKeyDown(SDL_Keycode keycode);
    bool wasKeyDown(SDL_Keycode keycode);
    bool isKeyPressed(SDL_Keycode keycode);

    bool isMouseDown(uint8_t button);
    bool wasMouseDown(uint8_t button);
    bool isMousePressed(uint8_t button);

   private:
    friend class Application;
    void tick();

    void pressKey(SDL_Keycode keycode);
    void releaseKey(SDL_Keycode keycode);

    void pressMouseButton(uint8_t button);
    void releaseMouseButton(uint8_t button);

    InputManager() = default;
    ~InputManager() = default;

    InputManager(const InputManager &) = delete;
    InputManager &operator=(const InputManager &) = delete;

    std::unordered_map<SDL_Keycode, bool> m_keyMap;
    std::unordered_map<SDL_Keycode, bool> m_lastKeyMap;

    std::unordered_map<uint8_t, bool> m_mouseBtnMap;
    std::unordered_map<uint8_t, bool> m_lastMouseBtnMap;
};

}  // namespace sd

#endif /* SD_INPUT_MANAGER */
