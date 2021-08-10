#include "EditorLayer.hpp"
#include "Renderer/Renderer.hpp"
#include <imgui.h>

EditorLayer::EditorLayer() : sd::Layer("Editor Layer") { setBlockEvent(true); }

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

    m_defaultTarget = sd::Renderer::getDefaultTarget();
    sd::Renderer::setDefaultTarget(m_target);
}

void EditorLayer::onDetech() {
    m_texture.reset();
    m_target.reset();
    sd::Renderer::setDefaultTarget(m_defaultTarget);
}

void EditorLayer::onImGui() {
    sd::Renderer::setFramebuffer(nullptr);
    sd::Renderer::setClearColor(0.3, 0.3, 0.3, 1.0f);
    sd::Renderer::clear();
    ImGui::Begin("Scene");
    {
        ImGui::BeginChild("SceneRenderer");
        ImVec2 wsize = ImGui::GetWindowSize();
        // if game window not active, disable camera response
        // bool isFocus = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();
        if (m_width != wsize.x || m_height != wsize.y) {
            m_target->resize(wsize.x, wsize.y);
            m_texture->setPixels(wsize.x, wsize.y, nullptr);

            m_width = wsize.x;
            m_height = wsize.y;
        }
        // Copy the multisample texture to normal texture
        m_frameBuffer->copyFrom(m_target->getFramebuffer().get(),
                                sd::BufferBit::COLOR_BUFFER_BIT,
                                sd::TextureFilter::NEAREST);
        // Because I use the texture from OpenGL, I need to invert the V
        // from the UV.
        ImGui::Image((void *)(intptr_t)m_texture->getId(), wsize, ImVec2(0, 1),
                     ImVec2(1, 0));
        ImGui::EndChild();
    }
    ImGui::End();
}

void EditorLayer::onEventPoll(const SDL_Event &event) {
    if (event.type == SDL_KEYDOWN &&event.key.keysym.sym == SDLK_z) {
        SD_TRACE("z");
    }
}
