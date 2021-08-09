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

   private:
    sd::Ref<sd::Texture> m_texture;
    sd::Ref<sd::Framebuffer> m_frameBuffer;
    sd::Ref<sd::RenderTarget> m_target;
    int m_width;
    int m_height;
};

#endif /* EDTOR_LAYER_HPP */
