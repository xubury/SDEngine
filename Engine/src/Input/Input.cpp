#include "Input/Input.hpp"
#include <unordered_map>

namespace SD {

static std::unordered_map<SDL_Keycode, bool> s_keyMap;
static std::unordered_map<SDL_Keycode, bool> s_lastKeyMap;

static std::unordered_map<uint8_t, bool> s_mouseBtnMap;
static std::unordered_map<uint8_t, bool> s_lastMouseBtnMap;

static glm::vec2 m_mouseCoord;

bool Input::isKeyDown(SDL_Keycode keycode) {
    auto it = s_keyMap.find(keycode);
    if (it != s_keyMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool Input::wasKeyDown(SDL_Keycode keycode) {
    auto it = s_lastKeyMap.find(keycode);
    if (it != s_lastKeyMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool Input::isKeyPressed(SDL_Keycode keycode) {
    return isKeyDown(keycode) && !wasKeyDown(keycode);
}

bool Input::isMouseDown(uint8_t button) {
    auto it = s_mouseBtnMap.find(button);
    if (it != s_mouseBtnMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool Input::wasMouseDown(uint8_t button) {
    auto it = s_lastMouseBtnMap.find(button);
    if (it != s_lastMouseBtnMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool Input::isMousePressed(uint8_t button) {
    return isMouseDown(button) && !wasMouseDown(button);
}

glm::vec2 Input::getMouseCoord() { return m_mouseCoord; }

void Input::tick() {
    for (auto &[key, press] : s_keyMap) {
        s_lastKeyMap[key] = press;
    }
    for (auto &[button, press] : s_mouseBtnMap) {
        s_lastMouseBtnMap[button] = press;
    }
}

void Input::processEvent(const SDL_Event &event) {
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

void Input::pressKey(SDL_Keycode keycode) { s_keyMap[keycode] = true; }

void Input::releaseKey(SDL_Keycode keycode) { s_keyMap[keycode] = false; }

void Input::pressMouseButton(uint8_t button) {
    s_mouseBtnMap[button] = true;
}

void Input::releaseMouseButton(uint8_t button) {
    s_mouseBtnMap[button] = false;
}

void Input::setMouseCoord(float x, float y) {
    m_mouseCoord.x = x;
    m_mouseCoord.y = y;
}

}  // namespace SD
