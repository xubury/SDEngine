#ifndef SD_EDITOR_LAYER_HPP
#define SD_EDITOR_LAYER_HPP

#include "Core/Layer.hpp"
#include "Renderer/Camera.hpp"
#include "ScenePanel.hpp"
#include "CameraController.hpp"

#include "Renderer/System/ShadowSystem.hpp"
#include "Renderer/System/LightingSystem.hpp"
#include "Renderer/System/SkyboxSystem.hpp"
#include "Renderer/System/ProfileSystem.hpp"
#include "Renderer/System/PostProcessSystem.hpp"
#include "Renderer/System/SpriteRenderSystem.hpp"

namespace SD {

class EditorLayer : public Layer {
   public:
    EditorLayer(int width, int height);
    ~EditorLayer();

    void OnPush() override;

    void OnPop() override;

    void OnTick(float dt) override;

    void OnRender() override;

    void OnImGui() override;

    void OnEventProcess(const Event &event) override;

    void OnEventsProcess() override;

    void NewScene();
    void LoadScene();
    void SaveScene();

   private:
    void ProcessDialog();

    void Hide();

    void Show();

    ShadowSystem *m_shadowSystem;
    LightingSystem *m_lightingSystem;
    SkyboxSystem *m_skyboxSystem;
    SpriteRenderSystem *m_spriteSystem;
    PostProcessSystem *m_postProcessSystem;
    ProfileSystem *m_profileSystem;

    int m_width;
    int m_height;

    RenderTarget m_target;
    Ref<Framebuffer> m_screenBuffer;
    Ref<Framebuffer> m_debugGBuffer;

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
