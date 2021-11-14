#include "Input/Action.hpp"
#include "Input/Input.hpp"

namespace SD {

Action::Action(const Event::EventType &event) : m_type(Type::PRESSED) {
    m_event.type = event;
}

Action::Action(Keycode key, int type) : m_type(type) {
    m_event.key.keycode = key;
    m_event.type = Event::EventType::KEY_PRESSED;
}

Action::Action(MouseButton button, int type) : m_type(type) {
    m_event.mouseButton.button = button;
    m_event.type = Event::EventType::MOUSE_BUTTON_PRESSED;
}

bool Action::operator==(const Event &event) const {
    bool res = false;
    // TODO: add code for handling other event
    switch (event.type) {
        case Event::EventType::KEY_PRESSED: {
            if (m_type & Type::PRESSED &&
                m_event.type == Event::EventType::KEY_PRESSED)
                res = event.key.keycode == m_event.key.keycode;
        } break;
        case Event::EventType::KEY_RELEASED: {
            if (m_type & Type::RELEASED &&
                m_event.type == Event::EventType::KEY_PRESSED)
                res = event.key.keycode == m_event.key.keycode;
        } break;
        case Event::EventType::MOUSE_BUTTON_PRESSED: {
            if (m_type & Type::PRESSED &&
                m_event.type == Event::EventType::MOUSE_BUTTON_PRESSED)
                res = event.mouseButton.button == m_event.mouseButton.button;
        } break;
        case Event::EventType::MOUSE_BUTTON_RELEASED: {
            if (m_type & Type::RELEASED &&
                m_event.type == Event::EventType::MOUSE_BUTTON_PRESSED)
                res = event.mouseButton.button == m_event.mouseButton.button;
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
    if (m_event.type == Event::EventType::KEY_PRESSED) {
        if (m_type & Type::PRESSED) {
            res = Input::isKeyPressed(m_event.key.keycode);
        }
        if (m_type & Type::DOWN) {
            res = Input::isKeyDown(m_event.key.keycode);
        }
    } else if (m_event.type == Event::EventType::MOUSE_BUTTON_PRESSED) {
        if (m_type & Type::PRESSED) {
            res = Input::isMousePressed(m_event.mouseButton.button);
        }
        if (m_type & Type::DOWN) {
            res = Input::isMouseDown(m_event.mouseButton.button);
        }
    }
    return res;
}

}  // namespace SD
