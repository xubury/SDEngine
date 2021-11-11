#ifndef SD_EDITOR_LAYER_HPP
#define SD_EDITOR_LAYER_HPP

#include "Core/Layer.hpp"
#include "Renderer/Camera.hpp"
#include "ScenePanel.hpp"
#include "CameraController.hpp"

namespace SD {

class EditorLayer : public Layer {
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

    Ref<Framebuffer> m_screenBuffer;
    Ref<Framebuffer> m_debugGBuffer;

    int m_width;
    int m_height;

    glm::vec2 m_viewportBounds[2];
    bool m_isViewportFocused;
    bool m_isViewportHovered;

    bool m_hide;

    PerspectiveCamera m_editorCamera;
    CameraController m_cameraController;

    Ref<Scene> m_scene;
    ScenePanel m_scenePanel;

    bool m_loadSceneOpen;
    bool m_saveSceneOpen;
    ImFileDialogInfo m_fileDialogInfo;

    Ref<Texture> m_lightIcon;
};

}  // namespace SD

#endif /* EDITOR_LAYER_HPP */
