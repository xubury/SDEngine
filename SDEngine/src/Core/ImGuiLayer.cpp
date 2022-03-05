#include "Core/ImGuiLayer.hpp"
#include "Core/Window.hpp"
#include "Graphics/Device.hpp"
#include "ImGuizmo.h"

namespace SD {

ImGuiLayer::ImGuiLayer(Window* window)
    : Layer("ImGuiLayer"), m_window(window) {}

ImGuiLayer::~ImGuiLayer() { m_window->ImGuiShutdown(); }

void ImGuiLayer::Begin() {
    m_window->ImGuiNewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
    Device::Get().SetFramebuffer(nullptr);
}

void ImGuiLayer::End() {
    ImGui::Render();
    m_window->ImGuiRenderDrawData();
}

void ImGuiLayer::OnInit() {
    Layer::OnInit();

    IMGUI_CHECKVERSION();
    m_context = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    // Fonts
    int pixel_size = 18.f;
    // io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf",
    //                              pixel_size, nullptr,
    //                              io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    io.Fonts->AddFontFromFileTTF(
        "assets/fonts/msyh.ttc", pixel_size, nullptr,
        io.Fonts->GetGlyphRangesChineseSimplifiedCommon());

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    SetDarkThemeColor();

    // Setup Platform/Renderer backends
    m_window->ImGuiInitBackend();
}

void ImGuiLayer::OnPush() {
    ImGuiIO& io = ImGui::GetIO();
    m_handlers.push_back(EventSystem::Get().Register<WindowSizeEvent>(
        [&](const WindowSizeEvent& e) {
            io.DisplaySize.x = static_cast<float>(e.width);
            io.DisplaySize.y = static_cast<float>(e.height);
        }));
    m_handlers.push_back(EventSystem::Get().Register<TextInputEvent>(
        [&](const TextInputEvent& e) { io.AddInputCharactersUTF8(e.text); }));
    m_handlers.push_back(
        EventSystem::Get().Register<KeyEvent>([&](const KeyEvent& e) {
            io.KeysDown[static_cast<uint16_t>(
                GetScancodeFromKeycode(e.keycode))] = e.state;
            io.KeyShift = e.mod == Keymod::SHIFT;
            io.KeyCtrl = e.mod == Keymod::CTRL;
            io.KeyAlt = e.mod == Keymod::ALT;
            io.KeySuper = e.mod == Keymod::GUI;
#if defined(SD_PLATFORM_WINDOWS)
            io.KeySuper = false;
#else
            io.KeySuper = e.mod == Keymod::GUI;
#endif
        }));
    m_handlers.push_back(EventSystem::Get().Register<MouseMotionEvent>(
        [&](const MouseMotionEvent& e) {
            io.MousePos.x = static_cast<float>(e.x);
            io.MousePos.y = static_cast<float>(e.y);
        }));
    m_handlers.push_back(EventSystem::Get().Register<MouseButtonEvent>(
        [&](const MouseButtonEvent& e) {
            int button = static_cast<int>(e.button) -
                         static_cast<int>(MouseButton::LEFT);
            io.MouseDown[button] = e.state;
        }));
    m_handlers.push_back(EventSystem::Get().Register<MouseWheelEvent>(
        [&](const MouseWheelEvent& e) { io.MouseWheel += e.y; }));
}

void ImGuiLayer::OnPop() { m_handlers.clear(); }

void ImGuiLayer::SetDarkThemeColor() {
    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.105f, 0.11f, 1.0f};

    // Headers
    colors[ImGuiCol_Header] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_HeaderHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_HeaderActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_ButtonHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_ButtonActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_FrameBgHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_FrameBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TabHovered] = ImVec4{0.38f, 0.3805f, 0.381f, 1.0f};
    colors[ImGuiCol_TabActive] = ImVec4{0.28f, 0.2805f, 0.281f, 1.0f};
    colors[ImGuiCol_TabUnfocused] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TitleBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
}

}  // namespace SD
