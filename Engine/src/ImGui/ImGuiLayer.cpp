#include "ImGui/ImGuiLayer.hpp"
#include "Core/Application.hpp"
#include "Graphics/Graphics.hpp"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "ImGuizmo.h"

namespace SD {

ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}

void ImGuiLayer::begin() {
    switch (getGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            ImGui_ImplOpenGL3_NewFrame();
            break;
        default:
            break;
    }
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
}

void ImGuiLayer::end() {
    ImGui::Render();
    switch (getGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            break;
        default:
            break;
    }
}

void ImGuiLayer::onPush() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    // Fonts
    io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf",
                                 18.0f);
    io.FontDefault = io.Fonts->AddFontFromFileTTF(
        "assets/fonts/opensans/OpenSans-Regular.ttf", 18.0f);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    setDarkThemeColor();

    // Setup Platform/Renderer backends
    switch (getGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            ImGui_ImplSDL2_InitForOpenGL(
                getApp().getWindow().getHandle(),
                getApp().getWindow().getGraphicsContext());
            ImGui_ImplOpenGL3_Init("#version 450");
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
    }
}

void ImGuiLayer::onPop() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
}

void ImGuiLayer::onEventProcess(const Event& event) {
    auto& io = ImGui::GetIO();
    switch (event.type) {
        default:
            break;
        case Event::EventType::WINDOW_RESIZED:
            io.DisplaySize.x = static_cast<float>(event.size.width);
            io.DisplaySize.y = static_cast<float>(event.size.height);
            break;
        case Event::EventType::TEXT_INPUT: {
            // TODO: not sure if it is correct
            io.AddInputCharacterUTF16(*event.text.text);
        } break;
        case Event::EventType::KEY_PRESSED: {
            io.KeysDown[static_cast<uint16_t>(
                getScancodeFromKeycode(event.key.keycode))] = true;
            io.KeyShift = event.key.mod == Keymod::SHIFT;
            io.KeyCtrl = event.key.mod == Keymod::CTRL;
            io.KeyAlt = event.key.mod == Keymod::ALT;
            io.KeySuper = event.key.mod == Keymod::GUI;
        } break;
        case Event::EventType::KEY_RELEASED: {
            io.KeysDown[static_cast<uint16_t>(
                getScancodeFromKeycode(event.key.keycode))] = true;
            io.KeyShift = event.key.mod == Keymod::SHIFT;
            io.KeyCtrl = event.key.mod == Keymod::CTRL;
            io.KeyAlt = event.key.mod == Keymod::ALT;
            io.KeySuper = event.key.mod == Keymod::GUI;
        } break;
        case Event::EventType::MOUSE_MOTION: {
            glm::vec2 screenPos =
                glm::vec2(event.mouseMotion.x, event.mouseMotion.y);
            io.MousePos.x = static_cast<float>(screenPos.x);
            io.MousePos.y = static_cast<float>(screenPos.y);
        } break;
        case Event::EventType::MOUSE_BUTTON_PRESSED: {
            int button = static_cast<int>(event.mouseButton.button) -
                         static_cast<int>(MouseButton::LEFT);
            if (io.WantCaptureMouse) {
                io.MouseDown[button] = true;
            }
        } break;
        case Event::EventType::MOUSE_BUTTON_RELEASED: {
            int button = static_cast<int>(event.mouseButton.button) -
                         static_cast<int>(MouseButton::LEFT);
            if (io.WantCaptureMouse) {
                io.MouseDown[button] = false;
            }
        } break;
        case Event::EventType::MOUSE_WHEEL_SCROLLED: {
            if (io.WantCaptureMouse) {
                io.MouseWheel += event.mouseWheel.y;
            }
        } break;
    }
}

void ImGuiLayer::setDarkThemeColor() {
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
