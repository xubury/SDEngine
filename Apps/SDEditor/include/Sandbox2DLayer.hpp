#ifndef SANDBOX_2D_LAYER_HPP
#define SANDBOX_2D_LAYER_HPP

#include "Core/Layer.hpp"
#include "Renderer/RenderTarget.hpp"
#include "Core/ActionTarget.hpp"
#include "ECS/ParticleSystem.hpp"
#include "Graphics/Model.hpp"

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
    sd::ActionTarget<int> m_actionTarget;
    sd::ActionMap<int> m_actionMap;

    sd::ParticleSystem *m_particleSystem;

    sd::OrthographicCamera m_masterCam;
};

#endif /* SANDBOX_2D_LAYER_HPP */
