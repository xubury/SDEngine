#include "Input/InputEngine.hpp"
#include <unordered_map>

namespace sd {

static std::unordered_map<SDL_Keycode, bool> s_keyMap;
static std::unordered_map<SDL_Keycode, bool> s_lastKeyMap;

static std::unordered_map<uint8_t, bool> s_mouseBtnMap;
static std::unordered_map<uint8_t, bool> s_lastMouseBtnMap;

static glm::vec2 m_mouseCoord;

bool InputEngine::isKeyDown(SDL_Keycode keycode) {
    auto it = s_keyMap.find(keycode);
    if (it != s_keyMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool InputEngine::wasKeyDown(SDL_Keycode keycode) {
    auto it = s_lastKeyMap.find(keycode);
    if (it != s_lastKeyMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool InputEngine::isKeyPressed(SDL_Keycode keycode) {
    return isKeyDown(keycode) && !wasKeyDown(keycode);
}

bool InputEngine::isMouseDown(uint8_t button) {
    auto it = s_mouseBtnMap.find(button);
    if (it != s_mouseBtnMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool InputEngine::wasMouseDown(uint8_t button) {
    auto it = s_lastMouseBtnMap.find(button);
    if (it != s_lastMouseBtnMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool InputEngine::isMousePressed(uint8_t button) {
    return isMouseDown(button) && !wasMouseDown(button);
}

glm::vec2 InputEngine::getMouseCoord() { return m_mouseCoord; }

void InputEngine::tick() {
    for (auto &[key, press] : s_keyMap) {
        s_lastKeyMap[key] = press;
    }
    for (auto &[button, press] : s_mouseBtnMap) {
        s_lastMouseBtnMap[button] = press;
    }
}

void InputEngine::processEvent(const SDL_Event &event) {
    switch (event.type) {
        case SDL_KEYDOWN:
            pressKey(event.key.keysym.sym);
            break;
        case SDL_KEYUP:
            releaseKey(event.key.keysym.sym);
            break;
        case SDL_MOUSEBUTTONDOWN:
            pressMouseButton(event.button.button);
            break;
        case SDL_MOUSEBUTTONUP:
            releaseMouseButton(event.button.button);
            break;
        case SDL_MOUSEMOTION:
            setMouseCoord(event.motion.x, event.motion.y);
            break;
    }
}

void InputEngine::pressKey(SDL_Keycode keycode) { s_keyMap[keycode] = true; }

void InputEngine::releaseKey(SDL_Keycode keycode) { s_keyMap[keycode] = false; }

void InputEngine::pressMouseButton(uint8_t button) {
    s_mouseBtnMap[button] = true;
}

void InputEngine::releaseMouseButton(uint8_t button) {
    s_mouseBtnMap[button] = false;
}

void InputEngine::setMouseCoord(float x, float y) {
    m_mouseCoord.x = x;
    m_mouseCoord.y = y;
}

}  // namespace sd
