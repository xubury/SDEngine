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
    EditorLayer(int width, int height, int msaa);
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

    int GetViewportWidth() const { return m_width; }
    int GetViewportHeight() const { return m_height; }

   private:
    void ProcessDialog();

    void Hide();

    void Show();

    ShadowSystem *m_shadow_system;
    LightingSystem *m_lighting_system;
    SkyboxSystem *m_skybox_system;
    SpriteRenderSystem *m_sprite_system;
    PostProcessSystem *m_post_process_system;
    ProfileSystem *m_profile_system;

    int m_width;
    int m_height;

    RenderTarget m_target;
    Ref<Framebuffer> m_screen_buffer;
    Ref<Framebuffer> m_debug_gbuffer;

    glm::vec2 m_viewport_bounds[2];
    bool m_is_viewport_focused;
    bool m_is_viewport_hovered;

    bool m_hide;

    PerspectiveCamera m_editor_camera;
    CameraController m_camera_controller;

    Ref<Scene> m_scene;
    ScenePanel m_scene_panel;

    bool m_load_scene_open;
    bool m_save_scene_open;
    ImFileDialogInfo m_file_dialog_info;

    Ref<Texture> m_light_icon;
};

}  // namespace SD

#endif /* EDITOR_LAYER_HPP */
