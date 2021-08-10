#include "Core/Action.hpp"
#include "Core/InputManager.hpp"

namespace sd {

Action::Action(const SDL_EventType &event) : m_type(Type::PRESSED) {
    m_event.type = event;
}

Action::Action(SDL_Keycode key, int type) : m_type(type) {
    m_event.key.keysym.sym = key;
    m_event.type = SDL_KEYDOWN;
}

Action::Action(uint8_t button, int type) : m_type(type) {
    m_event.button.button = button;
    m_event.type = SDL_MOUSEBUTTONDOWN;
}

bool Action::operator==(const SDL_Event &event) const {
    bool res = false;
    // TODO: add code for handling other event
    switch (event.type) {
        case SDL_KEYDOWN: {
            if (m_type & Type::PRESSED && m_event.type == SDL_KEYDOWN)
                res = event.key.keysym.sym == m_event.key.keysym.sym;
        } break;
        case SDL_KEYUP: {
            if (m_type & Type::RELEASED && m_event.type == SDL_KEYDOWN)
                res = event.key.keysym.sym == m_event.key.keysym.sym;
        } break;
        case SDL_MOUSEBUTTONDOWN: {
            if (m_type & Type::PRESSED && m_event.type == SDL_MOUSEBUTTONDOWN)
                res = event.button.button == m_event.button.button;
        } break;
        case SDL_MOUSEBUTTONUP: {
            if (m_type & Type::RELEASED && m_event.type == SDL_MOUSEBUTTONDOWN)
                res = event.button.button == m_event.button.button;
        } break;
        default:
            res = m_event.type == event.type;
            break;
    }
    return res;
}

bool Action::test() const {
    bool res = false;
    // TODO:other detect (joystick) need to be implemented
    if (m_event.type == SDL_KEYDOWN) {
        if (m_type & Type::PRESSED) {
            res = InputManager::isKeyPressed(m_event.key.keysym.sym);
        }
        if (m_type & Type::DOWN) {
            res = InputManager::isKeyDown(m_event.key.keysym.sym);
        }
    } else if (m_event.type == SDL_MOUSEBUTTONDOWN) {
        if (m_type & Type::PRESSED) {
            res = InputManager::isMousePressed(m_event.button.button);
        }
        if (m_type & Type::DOWN) {
            res = InputManager::isMouseDown(m_event.key.keysym.sym);
        }
    }
    return res;
}

}  // namespace sd
