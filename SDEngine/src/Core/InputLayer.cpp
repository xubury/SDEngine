#include "Core/InputLayer.hpp"
#include "Core/Input.hpp"

namespace SD {

InputLayer::InputLayer() : Layer("InputLayer") {}

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

void InputLayer::On(const MouseWheelEvent &event)
{
    Input::SetMouseWheel(event.x, event.y);
}

}  // namespace SD
