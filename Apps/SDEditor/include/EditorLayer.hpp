#ifndef EDITOR_LAYER_HPP
#define EDITOR_LAYER_HPP

#include "Core/Layer.hpp"
#include "Graphics/Camera.hpp"
#include "Renderer/RenderTarget.hpp"
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

    void onEventProcess(const SDL_Event &event) override;

    void onEventsProcess() override;

    void newScene();
    void openScene();
    void saveScene();

   private:
    void hide();

    void show();

    // The multisampling texture render target.
    sd::RenderTarget m_target;

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

    float m_fov;
    float m_nearZ;
    float m_farZ;
    sd::PerspectiveCamera m_editorCamera;
    CameraController m_cameraController;

    sd::Ref<sd::Scene> m_scene;
    ScenePanel m_scenePanel;
};

#endif /* EDITOR_LAYER_HPP */
