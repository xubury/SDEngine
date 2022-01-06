#include "Core/ImGuiLayer.hpp"
#include "Graphics/Graphics.hpp"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "ImGuizmo.h"

namespace SD {

ImGuiLayer::ImGuiLayer(Window* window)
    : Layer("ImGuiLayer"), m_window(window) {}

ImGuiLayer::~ImGuiLayer() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
}

void ImGuiLayer::Begin() {
    switch (GetGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            ImGui_ImplOpenGL3_NewFrame();
            break;
        default:
            break;
    }
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
    device->SetFramebuffer(nullptr);
}

void ImGuiLayer::End() {
    ImGui::Render();
    switch (GetGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            break;
        default:
            break;
    }
}

void ImGuiLayer::OnInit() {
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
    switch (GetGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            ImGui_ImplSDL2_InitForOpenGL(
                static_cast<SDL_Window*>(m_window->GetHandle()),
                m_window->GetGraphicsContext());
            ImGui_ImplOpenGL3_Init("#version 450");
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
    }
}

void ImGuiLayer::OnPush() {}

void ImGuiLayer::OnPop() {}

void ImGuiLayer::OnEventProcess(const Event& event) {
    auto& io = ImGui::GetIO();
    switch (event.type) {
        default:
            break;
        case EventType::WINDOW_RESIZED:
            io.DisplaySize.x = static_cast<float>(event.window_size.width);
            io.DisplaySize.y = static_cast<float>(event.window_size.height);
            break;
        case EventType::TEXT_INPUT: {
            // TODO: not sure if it is correct
            io.AddInputCharactersUTF8(event.text_input.text);
        } break;
        case EventType::KEY: {
            io.KeysDown[static_cast<uint16_t>(
                GetScancodeFromKeycode(event.key.keycode))] = event.key.state;
            io.KeyShift = event.key.mod == Keymod::SHIFT;
            io.KeyCtrl = event.key.mod == Keymod::CTRL;
            io.KeyAlt = event.key.mod == Keymod::ALT;
            io.KeySuper = event.key.mod == Keymod::GUI;
#if defined(SD_PLATFORM_WINDOWS)
            io.KeySuper = false;
#else
            io.KeySuper = event.key.mod == Keymod::GUI;
#endif
        } break;
        case EventType::MOUSE_MOTION: {
            io.MousePos.x = static_cast<float>(event.mouse_motion.x);
            io.MousePos.y = static_cast<float>(event.mouse_motion.y);
        } break;
        case EventType::MOUSE_BUTTON: {
            int button = static_cast<int>(event.mouse_button.button) -
                         static_cast<int>(MouseButton::LEFT);
            io.MouseDown[button] = event.mouse_button.state;
        } break;
        case EventType::MOUSE_WHEEL_SCROLLED: {
            io.MouseWheel += event.mouse_wheel.y;
        } break;
    }
}

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
