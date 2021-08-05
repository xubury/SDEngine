#ifndef SANDBOX_2D_LAYER_HPP
#define SANDBOX_2D_LAYER_HPP

#include "Core/Layer.hpp"
#include "Graphics/RenderTarget.hpp"

class Sandbox2DLayer : public sd::Layer {
   public:
    Sandbox2DLayer();

    ~Sandbox2DLayer() = default;

    void onAttach() override;

    void onTick(float dt) override;

    void onRender() override;

    void onEventPoll(const SDL_Event &) override;

    void onEventProcess() override;

   private:
    sd::RenderTarget m_target;
    sd::RenderTarget m_defaultTarget;
    sd::Ref<sd::Texture> m_texture;
};

#endif /* SANDBOX_2D_LAYER_HPP */
