#include "Core/InputEngine.hpp"

namespace sd {

InputEngine::InputEngine() : Layer("InputEngine") {}

void InputEngine::onTick(float) {
    for (auto &[key, press] : m_keyMap) {
        m_lastKeyMap[key] = press;
    }
    for (auto &[button, press] : m_mouseBtnMap) {
        m_lastMouseBtnMap[button] = press;
    }
}

void InputEngine::onEventProcess(const SDL_Event &event) {
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

void InputEngine::pressKey(SDL_Keycode keycode) { m_keyMap[keycode] = true; }

void InputEngine::releaseKey(SDL_Keycode keycode) { m_keyMap[keycode] = false; }

void InputEngine::pressMouseButton(uint8_t button) {
    m_mouseBtnMap[button] = true;
}

void InputEngine::releaseMouseButton(uint8_t button) {
    m_mouseBtnMap[button] = false;
}

void InputEngine::setMouseCoord(float x, float y) {
    m_mouseCoord.x = x;
    m_mouseCoord.y = y;
}

}  // namespace sd
