#include "Input/Input.hpp"
#include <unordered_map>

namespace SD {

static std::unordered_map<Keycode, bool> s_keyMap;
static std::unordered_map<Keycode, bool> s_lastKeyMap;

static std::unordered_map<MouseButton, bool> s_mouseBtnMap;
static std::unordered_map<MouseButton, bool> s_lastMouseBtnMap;

static glm::vec2 s_mouseCoord;

bool Input::IsKeyDown(Keycode keycode) {
    auto it = s_keyMap.find(keycode);
    if (it != s_keyMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool Input::WasKeyDown(Keycode keycode) {
    auto it = s_lastKeyMap.find(keycode);
    if (it != s_lastKeyMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool Input::IsKeyPressed(Keycode keycode) {
    return IsKeyDown(keycode) && !WasKeyDown(keycode);
}

bool Input::IsMouseDown(MouseButton button) {
    auto it = s_mouseBtnMap.find(button);
    if (it != s_mouseBtnMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool Input::WasMouseDown(MouseButton button) {
    auto it = s_lastMouseBtnMap.find(button);
    if (it != s_lastMouseBtnMap.end()) {
        return it->second;
    } else {
        return false;
    }
}

bool Input::IsMousePressed(MouseButton button) {
    return IsMouseDown(button) && !WasMouseDown(button);
}

glm::vec2 Input::GetMouseCoord() { return s_mouseCoord; }

void Input::Tick() {
    for (auto &[key, press] : s_keyMap) {
        s_lastKeyMap[key] = press;
    }
    for (auto &[button, press] : s_mouseBtnMap) {
        s_lastMouseBtnMap[button] = press;
    }
}

void Input::PressKey(Keycode keycode) { s_keyMap[keycode] = true; }

void Input::ReleaseKey(Keycode keycode) { s_keyMap[keycode] = false; }

void Input::PressMouseButton(MouseButton button) {
    s_mouseBtnMap[button] = true;
}

void Input::ReleaseMouseButton(MouseButton button) {
    s_mouseBtnMap[button] = false;
}

void Input::SetMouseCoord(float x, float y) {
    s_mouseCoord.x = x;
    s_mouseCoord.y = y;
}

}  // namespace SD
