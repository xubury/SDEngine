#ifndef EDITOR_LAYER_HPP
#define EDITOR_LAYER_HPP

#include "Core/Layer.hpp"
#include "Graphics/Camera.hpp"
#include "Renderer/RenderTarget.hpp"
#include "ECS/RenderSystem.hpp"
#include "ScenePanel.hpp"
#include "CameraController.hpp"

class EditorLayer : public sd::Layer {
   public:
    EditorLayer();

    void onAttach() override;

    void onDetech() override;

    void onTick(float dt) override;

    void onRender() override;

    void onImGui() override;

    void onEventPoll(const SDL_Event &event) override;

    void onEventProcess() override;

    void newScene();
    void openScene();
    void saveScene();

   private:
    void hide();

    void show();

    // The multisampling texture render target.
    sd::Ref<sd::RenderTarget> m_target;

    // The framebuffer and texture for copying multisampling texture.
    // You can't directly render multisampling texture.
    sd::Ref<sd::Framebuffer> m_frameBuffer;
    sd::Ref<sd::Texture> m_texture;

    int m_width;
    int m_height;

    glm::vec2 m_viewportBounds[2];
    bool m_isViewportFocused;
    bool m_isViewportHovered;

    bool m_hide;

    sd::PerspectiveCamera m_editorCamera;

    sd::Ref<sd::RenderSystem> m_renderSystem;

    sd::Ref<sd::Scene> m_scene;
    ScenePanel m_scenePanel;

    CameraController m_cameraController;
};

#endif /* EDITOR_LAYER_HPP */
