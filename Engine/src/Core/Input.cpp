#include "Core/Input.hpp"

namespace sd {

Input &Input::manager() {
    static Input s_instance;
    return s_instance;
}

void Input::tick() {
    for (auto &[key, press] : m_keyMap) {
        m_lastKeyMap[key] = press;
    }
    for (auto &[button, press] : m_mouseBtnMap) {
        m_lastMouseBtnMap[button] = press;
    }
}

void Input::pressKey(SDL_Keycode keycode) { m_keyMap[keycode] = true; }

void Input::releaseKey(SDL_Keycode keycode) {
    m_keyMap[keycode] = false;
}

void Input::pressMouseButton(uint8_t button) {
    m_mouseBtnMap[button] = true;
}

void Input::releaseMouseButton(uint8_t button) {
    m_mouseBtnMap[button] = false;
}

bool Input::isKeyDown(SDL_Keycode keycode) {
    auto it = manager().m_keyMap.find(keycode);
    if (it != manager().m_keyMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool Input::wasKeyDown(SDL_Keycode keycode) {
    auto it = manager().m_lastKeyMap.find(keycode);
    if (it != manager().m_lastKeyMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool Input::isKeyPressed(SDL_Keycode keycode) {
    return isKeyDown(keycode) && !wasKeyDown(keycode);
}

bool Input::isMouseDown(uint8_t button) {
    auto it = manager().m_mouseBtnMap.find(button);
    if (it != manager().m_mouseBtnMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool Input::wasMouseDown(uint8_t button) {
    auto it = manager().m_lastMouseBtnMap.find(button);
    if (it != manager().m_lastMouseBtnMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool Input::isMousePressed(uint8_t button) {
    return isMouseDown(button) && !wasMouseDown(button);
}

void Input::setMouseCoord(float x, float y) {
    m_mouseCoord.x = x;
    m_mouseCoord.y = y;
}

glm::vec2 Input::getMouseCoord() { return manager().m_mouseCoord; }

}  // namespace sd
