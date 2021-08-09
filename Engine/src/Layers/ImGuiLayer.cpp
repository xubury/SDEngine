#include "Layers/ImGuiLayer.hpp"
#include "Core/Application.hpp"
#include "Graphics/Graphics.hpp"
#include "Renderer/Renderer.hpp"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

namespace sd {

ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer"), m_width(0), m_height(0) {}

void ImGuiLayer::begin() {
    switch (Renderer::getAPI()) {
        case API::OpenGL:
            ImGui_ImplOpenGL3_NewFrame();
            break;
        default:
            break;
    }
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::end() {
    ImGui::Render();
    Renderer::setClearColor(0.3, 0.3, 0.3, 1.0f);
    Renderer::clear();
    switch (Renderer::getAPI()) {
        case API::OpenGL:
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            break;
        default:
            break;
    }
}

void ImGuiLayer::onAttach() {
    m_texture = sd::Texture::create(
        1024, 1024, sd::TextureType::TEX_2D, sd::TextureFormat::RGBA,
        sd::TextureFormatType::UBYTE, sd::TextureWrap::BORDER,
        sd::TextureFilter::NEAREST, sd::TextureMipmapFilter::NEAREST);
    m_target = createRef<RenderTarget>();
    m_target->addTexture(m_texture);
    m_target->addTexture(sd::Texture::create(
        m_texture->getWidth(), m_texture->getHeight(), sd::TextureType::TEX_2D,
        sd::TextureFormat::DEPTH, sd::TextureFormatType::FLOAT,
        sd::TextureWrap::BORDER, sd::TextureFilter::NEAREST,
        sd::TextureMipmapFilter::NEAREST));
    m_target->init();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf",
                                 18.0f);
    io.FontDefault = io.Fonts->AddFontFromFileTTF(
        "assets/fonts/opensans/OpenSans-Regular.ttf", 18.0f);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    setDarkThemeColor();

    // Setup Platform/Renderer backends
    switch (Renderer::getAPI()) {
        case API::OpenGL:
            ImGui_ImplSDL2_InitForOpenGL(
                Application::getWindow().getSDLHandle(),
                Application::getWindow().getSDLGLContext());
            ImGui_ImplOpenGL3_Init("#version 450");
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
    }

    setBlockEvent(true);
    Renderer::setDefaultTarget(m_target);
}

void ImGuiLayer::onDetech() {}

void ImGuiLayer::onImGui() {
    Renderer::setFramebuffer(nullptr);
    ImGui::Begin("Scene");
    {
        ImGui::BeginChild("SceneRenderer");
        ImVec2 wsize = ImGui::GetWindowSize();
        // if game window not active, disable camera response
        // bool isFocus = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();
        if (m_width != wsize.x || m_height != wsize.y) {
            m_target->resize(wsize.x, wsize.y);
            m_width = wsize.x;
            m_height = wsize.y;
        }
        // Because I use the texture from OpenGL, I need to invert the V
        // from the UV.
        ImGui::Image((void*)(intptr_t)m_texture->getId(), wsize, ImVec2(0, 1),
                     ImVec2(1, 0));
        ImGui::EndChild();
    }
    ImGui::End();
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

}  // namespace sd
