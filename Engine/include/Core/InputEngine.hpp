#ifndef SD_INPUT_ENGINE_HPP
#define SD_INPUT_ENGINE_HPP

#include <glm/glm.hpp>
#include "Utility/Export.hpp"
#include "Core/Layer.hpp"

namespace sd {

class SD_API InputEngine : public Layer {
   public:
    InputEngine();
    ~InputEngine() = default;

    void onTick(float dt) override;

    void onEventProcess(const SDL_Event &event) override;

   private:
    friend class Input;

    void pressKey(SDL_Keycode keycode);
    void releaseKey(SDL_Keycode keycode);

    void pressMouseButton(uint8_t button);
    void releaseMouseButton(uint8_t button);

    void setMouseCoord(float x, float y);

    std::unordered_map<SDL_Keycode, bool> m_keyMap;
    std::unordered_map<SDL_Keycode, bool> m_lastKeyMap;

    std::unordered_map<uint8_t, bool> m_mouseBtnMap;
    std::unordered_map<uint8_t, bool> m_lastMouseBtnMap;

    glm::vec2 m_mouseCoord;
};

}  // namespace sd

#endif /* SD_INPUT_ENGINE_HPP */
