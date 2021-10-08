#include "Core/Input.hpp"
#include "Core/Application.hpp"

namespace sd {

bool Input::isKeyDown(SDL_Keycode keycode) {
    auto it = Application::getInputEngine().m_keyMap.find(keycode);
    if (it != Application::getInputEngine().m_keyMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool Input::wasKeyDown(SDL_Keycode keycode) {
    auto it = Application::getInputEngine().m_lastKeyMap.find(keycode);
    if (it != Application::getInputEngine().m_lastKeyMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool Input::isKeyPressed(SDL_Keycode keycode) {
    return isKeyDown(keycode) && !wasKeyDown(keycode);
}

bool Input::isMouseDown(uint8_t button) {
    auto it = Application::getInputEngine().m_mouseBtnMap.find(button);
    if (it != Application::getInputEngine().m_mouseBtnMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool Input::wasMouseDown(uint8_t button) {
    auto it = Application::getInputEngine().m_lastMouseBtnMap.find(button);
    if (it != Application::getInputEngine().m_lastMouseBtnMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool Input::isMousePressed(uint8_t button) {
    return isMouseDown(button) && !wasMouseDown(button);
}

glm::vec2 Input::getMouseCoord() {
    return Application::getInputEngine().m_mouseCoord;
}

}  // namespace sd
