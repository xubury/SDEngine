#include "EditorLayer.hpp"
#include "Renderer/Renderer.hpp"
#include "Core/Application.hpp"
#include "imgui.h"

EditorLayer::EditorLayer()
    : sd::Layer("Editor Layer"), m_width(0), m_height(0), m_hide(false) {}

void EditorLayer::onAttach() {
    sd::Ref<sd::Texture> multisampleTexture = sd::Texture::create(
        1024, 1024, 8, sd::TextureType::TEX_2D_MULTISAMPLE,
        sd::TextureFormat::RGBA, sd::TextureFormatType::UBYTE,
        sd::TextureWrap::BORDER, sd::TextureFilter::NEAREST,
        sd::TextureMipmapFilter::NEAREST);
    m_target = sd::createRef<sd::RenderTarget>();
    m_target->addTexture(multisampleTexture);
    m_target->addTexture(sd::Texture::create(
        multisampleTexture->getWidth(), multisampleTexture->getHeight(), 8,
        sd::TextureType::TEX_2D_MULTISAMPLE, sd::TextureFormat::DEPTH,
        sd::TextureFormatType::FLOAT, sd::TextureWrap::BORDER,
        sd::TextureFilter::NEAREST, sd::TextureMipmapFilter::NEAREST));
    m_target->init();

    m_texture = sd::Texture::create(
        1024, 1024, 1, sd::TextureType::TEX_2D, sd::TextureFormat::RGBA,
        sd::TextureFormatType::UBYTE, sd::TextureWrap::BORDER,
        sd::TextureFilter::NEAREST, sd::TextureMipmapFilter::NEAREST);

    m_frameBuffer = sd::Framebuffer::create();
    m_frameBuffer->attachTexture(m_texture.get());
    m_frameBuffer->setDrawable({0});
    show();
}

void EditorLayer::onDetech() {
    hide();
    m_texture.reset();
    m_target.reset();
}

void EditorLayer::onImGui() {
    if (m_hide) {
        return;
    }
    sd::Renderer::setFramebuffer(nullptr);
    sd::Renderer::setClearColor(0.3, 0.3, 0.3, 1.0f);
    sd::Renderer::clear();

    static bool dockspaceOpen = true;
    static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
    static bool fullScreen = true;

    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (fullScreen) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        windowFlags |= ImGuiWindowFlags_NoTitleBar |
                       ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                       ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
                       ImGuiWindowFlags_NoNavFocus;
    }

    if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
        windowFlags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspaceOpen, windowFlags);
    ImGui::PopStyleVar();

    if (fullScreen) {
        ImGui::PopStyleVar(2);
    }

    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = 370.0f;
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);
    }

    style.WindowMinSize.x = minWinSizeX;

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New", "Ctrl+N")) {
            }

            if (ImGui::MenuItem("Open...", "Ctrl+O")) {
            }

            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
            }

            if (ImGui::MenuItem("Exit")) {
                sd::Application::instance().quit();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Scene");
    {
        ImVec2 wsize = ImGui::GetContentRegionAvail();
        // if game window not active, disable camera response
        // bool isFocus = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();
        if (m_width != wsize.x || m_height != wsize.y) {
            if (wsize.x > 0 && wsize.y > 0) {
                m_target->resize(wsize.x, wsize.y);
                m_texture->setPixels(wsize.x, wsize.y, nullptr);
            }

            m_width = wsize.x;
            m_height = wsize.y;
        }
        // Copy the multisample texture to normal texture
        m_frameBuffer->copyFrom(m_target->getFramebuffer().get(),
                                sd::BufferBit::COLOR_BUFFER_BIT,
                                sd::TextureFilter::NEAREST);
        // Because I use the texture from OpenGL, I need to invert the V
        // from the UV.
        ImGui::Image((void*)(intptr_t)m_texture->getId(), wsize, ImVec2(0, 1),
                     ImVec2(1, 0));
    }
    ImGui::End();
    ImGui::PopStyleVar();

    ImGui::End();
}

void EditorLayer::hide() {
    m_hide = true;
    setBlockEvent(false);
    sd::Renderer::setDefaultTarget(m_defaultTarget);
}

void EditorLayer::show() {
    m_hide = false;
    setBlockEvent(true);
    m_defaultTarget = sd::Renderer::getDefaultTarget();
    sd::Renderer::setDefaultTarget(m_target);
}

void EditorLayer::onEventPoll(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_z) {
        if (m_hide) {
            show();
        } else {
            hide();
        }
    }
}
