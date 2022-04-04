#include "Core/Input.hpp"
#include "Core/Event.hpp"

namespace SD {

std::unordered_map<Keycode, bool> Input::s_key;
std::unordered_map<Keycode, bool> Input::s_last_key;

std::unordered_map<MouseButton, bool> Input::s_mouse_button;
std::unordered_map<MouseButton, bool> Input::s_last_mouse_button;

Vector2f Input::s_mouse_coord;

bool Input::IsKeyDown(Keycode keycode)
{
    auto it = s_key.find(keycode);
    if (it != s_key.end()) {
        return it->second;
    }
    else {
        return false;
    }
}

bool Input::WasKeyDown(Keycode keycode)
{
    auto it = s_last_key.find(keycode);
    if (it != s_last_key.end()) {
        return it->second;
    }
    else {
        return false;
    }
}

bool Input::IsKeyPressed(Keycode keycode)
{
    return IsKeyDown(keycode) && !WasKeyDown(keycode);
}

bool Input::IsMouseDown(MouseButton button)
{
    auto it = s_mouse_button.find(button);
    if (it != s_mouse_button.end()) {
        return it->second;
    }
    else {
        return false;
    }
}

bool Input::WasMouseDown(MouseButton button)
{
    auto it = s_last_mouse_button.find(button);
    if (it != s_last_mouse_button.end()) {
        return it->second;
    }
    else {
        return false;
    }
}

bool Input::IsMousePressed(MouseButton button)
{
    return IsMouseDown(button) && !WasMouseDown(button);
}

Vector2f Input::GetMouseCoord() { return s_mouse_coord; }

void Input::Tick()
{
    for (auto& [key, press] : s_key) {
        s_last_key[key] = press;
    }
    for (auto& [button, press] : s_mouse_button) {
        s_last_mouse_button[button] = press;
    }
}

void Input::SetKeyState(Keycode keycode, bool state) { s_key[keycode] = state; }

void Input::SetMouseButtonState(MouseButton button, bool state)
{
    s_mouse_button[button] = state;
}

void Input::SetMouseCoord(float x, float y)
{
    s_mouse_coord.x = x;
    s_mouse_coord.y = y;
}

}  // namespace SD
