#include "Core/InputLayer.hpp"
#include "Core/Input.hpp"

namespace SD {

InputLayer::InputLayer() : Layer("Input Layer") {}

void InputLayer::OnTick(float) { Input::Tick(); }

void InputLayer::On(const KeyEvent &event)
{
    Input::SetKeyState(event.keycode, event.state);
}

void InputLayer::On(const MouseMotionEvent &event)
{
    Input::SetMouseCoord(event.x, event.y);
}

void InputLayer::On(const MouseButtonEvent &event)
{
    Input::SetMouseCoord(event.x, event.y);
    Input::SetMouseButtonState(event.button, event.state);
}

void InputLayer::On(const MouseWheelEvent &) {}

}  // namespace SD
