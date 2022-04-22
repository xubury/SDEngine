#include "Core/InputDevice.hpp"

namespace SD {

bool InputDevice::IsKeyDown(Keycode keycode)
{
    auto it = m_key.find(keycode);
    if (it != m_key.end()) {
        return it->second;
    }
    else {
        return false;
    }
}

bool InputDevice::WasKeyDown(Keycode keycode)
{
    auto it = m_last_key.find(keycode);
    if (it != m_last_key.end()) {
        return it->second;
    }
    else {
        return false;
    }
}

bool InputDevice::IsKeyPressed(Keycode keycode)
{
    return IsKeyDown(keycode) && !WasKeyDown(keycode);
}

bool InputDevice::IsMouseDown(MouseButton button)
{
    auto it = m_mouse_button.find(button);
    if (it != m_mouse_button.end()) {
        return it->second;
    }
    else {
        return false;
    }
}

bool InputDevice::WasMouseDown(MouseButton button)
{
    auto it = m_last_mouse_button.find(button);
    if (it != m_last_mouse_button.end()) {
        return it->second;
    }
    else {
        return false;
    }
}

bool InputDevice::IsMousePressed(MouseButton button)
{
    return IsMouseDown(button) && !WasMouseDown(button);
}

Vector2f InputDevice::GetMouseCoord() { return m_mouse_coord; }

Vector2f InputDevice::GetMouseWheel() { return m_mouse_wheel; }

void InputDevice::Tick()
{
    for (auto& [key, press] : m_key) {
        m_last_key[key] = press;
    }
    for (auto& [button, press] : m_mouse_button) {
        m_last_mouse_button[button] = press;
    }
}

void InputDevice::SetKeyState(Keycode keycode, bool state) { m_key[keycode] = state; }

void InputDevice::SetMouseButtonState(MouseButton button, bool state)
{
    m_mouse_button[button] = state;
}

void InputDevice::SetMouseCoord(float x, float y)
{
    m_mouse_coord.x = x;
    m_mouse_coord.y = y;
}

void InputDevice::SetMouseWheel(float x, float y)
{
    m_mouse_wheel.x = x;
    m_mouse_wheel.y = y;
}

}  // namespace SD
