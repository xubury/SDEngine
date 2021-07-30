#ifndef SANDBOX_2D_LAYER_HPP
#define SANDBOX_2D_LAYER_HPP

#include "Core/Layer.hpp"
class Sandbox2DLayer : public sd::Layer {
   public:
    Sandbox2DLayer();
    ~Sandbox2DLayer() = default;

    void onTick(float dt) override;

    void onRender() override;

    void onEventPoll(const SDL_Event &) override;

    void onEventProcess() override;
};

#endif /* SANDBOX_2D_LAYER_HPP */
