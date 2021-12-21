#ifndef SD_EDITOR_LAYER_HPP
#define SD_EDITOR_LAYER_HPP

#include "Core/Layer.hpp"
#include "Renderer/Camera.hpp"
#include "ScenePanel.hpp"

#include "EditorCameraSystem.hpp"

#include "System/CameraSystem.hpp"
#include "System/LightingSystem.hpp"
#include "System/SkyboxSystem.hpp"
#include "System/ProfileSystem.hpp"
#include "System/PostProcessSystem.hpp"
#include "System/SpriteRenderSystem.hpp"
#include "System/TileMapSystem.hpp"

namespace SD {

class EditorLayer : public Layer {
   public:
    EditorLayer(int width, int height, int msaa);
    ~EditorLayer();

    void OnInit() override;

    void OnPush() override;

    void OnPop() override;

    void OnTick(float dt) override;

    void OnRender() override;

    void OnImGui() override;

    void OnEventProcess(const Event &event) override;

    void OnEventsProcess() override;

    void NewScene();
    void OpenLoadSceneDialog();
    void OpenSaveSceneDialog();

    int GetViewportWidth() const { return m_width; }
    int GetViewportHeight() const { return m_height; }

    void SetViewportSize(uint32_t left, uint32_t top, uint32_t width,
                         uint32_t height);

   private:
    void ProcessDialog();

    void Hide();

    void Show();

    ScenePanel *m_scene_panel;
    EditorCameraSystem *m_editor_camera_system;
    CameraSystem *m_camera_system;
    LightingSystem *m_lighting_system;
    SkyboxSystem *m_skybox_system;
    SpriteRenderSystem *m_sprite_system;
    PostProcessSystem *m_post_process_system;
    ProfileSystem *m_profile_system;
    TileMapSystem *m_tile_map_system;

    int m_width;
    int m_height;
    int m_msaa;

    Ref<Framebuffer> m_debug_gbuffer;
    Ref<Framebuffer> m_screen_buffer;

    Viewport m_viewport;
    bool m_is_viewport_focused;
    bool m_is_viewport_hovered;

    bool m_hide;

    bool m_load_scene_open;
    bool m_save_scene_open;
    ImFileDialogInfo m_file_dialog_info;

    Ref<Texture> m_light_icon;
};

}  // namespace SD

#endif /* EDITOR_LAYER_HPP */
