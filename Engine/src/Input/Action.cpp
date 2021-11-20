#include "Input/Action.hpp"
#include "Input/Input.hpp"

namespace SD {

Action::Action(const EventType &event) : m_type(Type::PRESSED) {
    m_event.type = event;
}

Action::Action(Keycode key, int type) : m_type(type) {
    m_event.key.keycode = key;
    m_event.type = EventType::KEY_PRESSED;
}

Action::Action(Scancode scancode, int type) : m_type(type) {
    m_event.key.keycode = GetKeycodeFromScancode(scancode);
    m_event.type = EventType::KEY_PRESSED;
}

Action::Action(MouseButton button, int type) : m_type(type) {
    m_event.mouse_button.button = button;
    m_event.type = EventType::MOUSE_BUTTON_PRESSED;
}

bool Action::operator==(const Event &event) const {
    bool res = false;
    // TODO: add code for handling other event
    switch (event.type) {
        case EventType::KEY_PRESSED: {
            if (m_type & Type::PRESSED &&
                m_event.type == EventType::KEY_PRESSED)
                res = event.key.keycode == m_event.key.keycode;
        } break;
        case EventType::KEY_RELEASED: {
            if (m_type & Type::RELEASED &&
                m_event.type == EventType::KEY_PRESSED)
                res = event.key.keycode == m_event.key.keycode;
        } break;
        case EventType::MOUSE_BUTTON_PRESSED: {
            if (m_type & Type::PRESSED &&
                m_event.type == EventType::MOUSE_BUTTON_PRESSED)
                res = event.mouse_button.button == m_event.mouse_button.button;
        } break;
        case EventType::MOUSE_BUTTON_RELEASED: {
            if (m_type & Type::RELEASED &&
                m_event.type == EventType::MOUSE_BUTTON_PRESSED)
                res = event.mouse_button.button == m_event.mouse_button.button;
        } break;
        default:
            res = m_event.type == event.type;
            break;
    }
    return res;
}

bool Action::Test() const {
    bool res = false;
    // TODO:other detect (joystick) need to be implemented
    if (m_event.type == EventType::KEY_PRESSED) {
        if (m_type & Type::PRESSED) {
            res = Input::IsKeyPressed(m_event.key.keycode);
        }
        if (m_type & Type::DOWN) {
            res = Input::IsKeyDown(m_event.key.keycode);
        }
    } else if (m_event.type == EventType::MOUSE_BUTTON_PRESSED) {
        if (m_type & Type::PRESSED) {
            res = Input::IsMousePressed(m_event.mouse_button.button);
        }
        if (m_type & Type::DOWN) {
            res = Input::IsMouseDown(m_event.mouse_button.button);
        }
    }
    return res;
}

}  // namespace SD
