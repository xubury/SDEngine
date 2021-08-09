#include "EditorLayer.hpp"
#include "Renderer/Renderer.hpp"
#include <imgui.h>

EditorLayer::EditorLayer() : sd::Layer("Editor Layer") {}

void EditorLayer::onAttach() {
    m_texture = sd::Texture::create(
        1024, 1024, sd::TextureType::TEX_2D, sd::TextureFormat::RGBA,
        sd::TextureFormatType::UBYTE, sd::TextureWrap::BORDER,
        sd::TextureFilter::NEAREST, sd::TextureMipmapFilter::NEAREST);
    m_target = sd::createRef<sd::RenderTarget>();
    m_target->addTexture(m_texture);
    m_target->addTexture(sd::Texture::create(
        m_texture->getWidth(), m_texture->getHeight(), sd::TextureType::TEX_2D,
        sd::TextureFormat::DEPTH, sd::TextureFormatType::FLOAT,
        sd::TextureWrap::BORDER, sd::TextureFilter::NEAREST,
        sd::TextureMipmapFilter::NEAREST));
    m_target->init();

    sd::Renderer::setDefaultTarget(m_target);
    setBlockEvent(true);
}

void EditorLayer::onDetech() {
    m_texture.reset();
    m_target.reset();
}

void EditorLayer::onImGui() {
    sd::Renderer::setFramebuffer(nullptr);
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
    sd::Renderer::setClearColor(0.3, 0.3, 0.3, 1.0f);
    sd::Renderer::clear();
}
