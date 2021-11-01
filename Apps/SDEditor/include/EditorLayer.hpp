#ifndef EDITOR_LAYER_HPP
#define EDITOR_LAYER_HPP

#include "Core/Layer.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/RenderTarget.hpp"
#include "ScenePanel.hpp"
#include "CameraController.hpp"

class EditorLayer : public sd::Layer {
   public:
    EditorLayer(int width, int height);

    void onAttach() override;

    void onDetech() override;

    void onTick(float dt) override;

    void onRender() override;

    void onImGui() override;

    void onEventProcess(const SDL_Event &event) override;

    void onEventsProcess() override;

    void newScene();
    void loadScene();
    void saveScene();

   private:
    void processDialog();

    void hide();

    void show();

    sd::Ref<sd::RenderTarget> m_target;

    int m_width;
    int m_height;

    glm::vec2 m_viewportBounds[2];
    bool m_isViewportFocused;
    bool m_isViewportHovered;

    bool m_hide;

    sd::PerspectiveCamera m_editorCamera;
    CameraController m_cameraController;

    sd::Ref<sd::Scene> m_scene;
    ScenePanel m_scenePanel;

    bool m_loadSceneOpen;
    bool m_saveSceneOpen;
    ImFileDialogInfo m_fileDialogInfo;

    sd::Ref<sd::Texture> m_lightIcon;
};

#endif /* EDITOR_LAYER_HPP */
