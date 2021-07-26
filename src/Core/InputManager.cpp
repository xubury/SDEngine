#include "Core/InputManager.hpp"

namespace sd {

InputManager &InputManager::instance() {
    static InputManager s_instance;
    return s_instance;
}

void InputManager::tick() {
    for (auto &[key, press] : m_keyMap) {
        m_lastKeyMap[key] = press;
    }
    for (auto &[button, press] : m_mouseBtnMap) {
        m_lastMouseBtnMap[button] = press;
    }
}

void InputManager::pressKey(SDL_Keycode keycode) { m_keyMap[keycode] = true; }

void InputManager::releaseKey(SDL_Keycode keycode) {
    m_keyMap[keycode] = false;
}

void InputManager::pressMouseButton(uint8_t button) {
    m_mouseBtnMap[button] = true;
}

void InputManager::releaseMouseButton(uint8_t button) {
    m_mouseBtnMap[button] = false;
}

bool InputManager::isKeyDown(SDL_Keycode keycode) {
    auto it = m_keyMap.find(keycode);
    if (it != m_keyMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool InputManager::wasKeyDown(SDL_Keycode keycode) {
    auto it = m_lastKeyMap.find(keycode);
    if (it != m_lastKeyMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool InputManager::isKeyPressed(SDL_Keycode keycode) {
    return isKeyDown(keycode) && !wasKeyDown(keycode);
}

bool InputManager::isMouseDown(uint8_t button) {
    auto it = m_mouseBtnMap.find(button);
    if (it != m_mouseBtnMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool InputManager::wasMouseDown(uint8_t button) {
    auto it = m_lastMouseBtnMap.find(button);
    if (it != m_lastMouseBtnMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool InputManager::isMousePressed(uint8_t button) {
    return isMouseDown(button) && !wasMouseDown(button);
}

}  // namespace sd
