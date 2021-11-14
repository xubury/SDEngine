#include "Input/Input.hpp"
#include <unordered_map>

namespace SD {

static std::unordered_map<Keycode, bool> s_keyMap;
static std::unordered_map<Keycode, bool> s_lastKeyMap;

static std::unordered_map<MouseButton, bool> s_mouseBtnMap;
static std::unordered_map<MouseButton, bool> s_lastMouseBtnMap;

static glm::vec2 s_mouseCoord;

bool Input::isKeyDown(Keycode keycode) {
    auto it = s_keyMap.find(keycode);
    if (it != s_keyMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool Input::wasKeyDown(Keycode keycode) {
    auto it = s_lastKeyMap.find(keycode);
    if (it != s_lastKeyMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool Input::isKeyPressed(Keycode keycode) {
    return isKeyDown(keycode) && !wasKeyDown(keycode);
}

bool Input::isMouseDown(MouseButton button) {
    auto it = s_mouseBtnMap.find(button);
    if (it != s_mouseBtnMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool Input::wasMouseDown(MouseButton button) {
    auto it = s_lastMouseBtnMap.find(button);
    if (it != s_lastMouseBtnMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool Input::isMousePressed(MouseButton button) {
    return isMouseDown(button) && !wasMouseDown(button);
}

glm::vec2 Input::getMouseCoord() { return s_mouseCoord; }

void Input::tick() {
    for (auto &[key, press] : s_keyMap) {
        s_lastKeyMap[key] = press;
    }
    for (auto &[button, press] : s_mouseBtnMap) {
        s_lastMouseBtnMap[button] = press;
    }
}

void Input::processEvent(const Event &event) {
    switch (event.type) {
        case Event::EventType::KEY_PRESSED:
            pressKey(event.key.keycode);
            break;
        case Event::EventType::KEY_RELEASED:
            releaseKey(event.key.keycode);
            break;
        case Event::EventType::MOUSE_BUTTON_PRESSED:
            pressMouseButton(event.mouseButton.button);
            break;
        case Event::EventType::MOUSE_BUTTON_RELEASED:
            releaseMouseButton(event.mouseButton.button);
            break;
        case Event::EventType::MOUSE_MOVED:
            setMouseCoord(event.mouseMove.xrel, event.mouseMove.yrel);
            break;
        case Event::EventType::MOUSE_WHEEL_SCROLLED:
            SD_CORE_ERROR("no impl mouse scroll");
            break;
        default:
            break;
    }
}

void Input::pressKey(Keycode keycode) { s_keyMap[keycode] = true; }

void Input::releaseKey(Keycode keycode) { s_keyMap[keycode] = false; }

void Input::pressMouseButton(MouseButton button) {
    s_mouseBtnMap[button] = true;
}

void Input::releaseMouseButton(MouseButton button) {
    s_mouseBtnMap[button] = false;
}

void Input::setMouseCoord(float x, float y) {
    s_mouseCoord.x = x;
    s_mouseCoord.y = y;
}

}  // namespace SD
