#include "Core/Window.hpp"
#include "Core/OpenGL/GLWindow.hpp"
#include "Graphics/Device.hpp"

namespace SD {

Scope<Window> Window::Create(const WindowCreateInfo &info) {
    SD_CORE_TRACE("Initializing Window...");
    Scope<Window> window;
    switch (Device::GetAPI()) {
        case Device::API::OpenGL:
            window = CreateScope<GLWindow>(info);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return window;
}

Window::Window() : m_should_close(false) {}

bool Window::ShouldClose() { return m_should_close; }

void Window::SetShouldClose(bool shouldClose) { m_should_close = shouldClose; }

}  // namespace SD
