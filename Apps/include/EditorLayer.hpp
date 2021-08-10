#ifndef EDTOR_LAYER_HPP
#define EDTOR_LAYER_HPP

#include "Core/Layer.hpp"
#include "Graphics/Texture.hpp"
#include "Renderer/RenderTarget.hpp"

class EditorLayer : public sd::Layer {
   public:
    EditorLayer();

    void onAttach() override;

    void onDetech() override;

    void onImGui() override;

    void onEventPoll(const SDL_Event &event) override;

   private:
    void hide();

    void show();

    // The multisampling texture render target.
    sd::Ref<sd::RenderTarget> m_target;

    // The framebuffer and texture for copying multisampling texture.
    // You can't directly render multisampling texture.
    sd::Ref<sd::Framebuffer> m_frameBuffer;
    sd::Ref<sd::Texture> m_texture;

    // This is used for restore the original render target
    sd::Ref<sd::RenderTarget> m_defaultTarget;
    int m_width;
    int m_height;

    bool m_hide;
};

#endif /* EDTOR_LAYER_HPP */
