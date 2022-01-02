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

enum EditorMode { NONE, TWO_DIMENSIONAL, THREE_DIMENSIONAL };

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

    void NewScene();
    void OpenLoadSceneDialog();
    void OpenSaveSceneDialog();

    int GetViewportWidth() const { return m_width; }
    int GetViewportHeight() const { return m_height; }

    void SetViewportSize(uint32_t left, uint32_t top, uint32_t width,
                         uint32_t height);

   private:
    void ProcessDialog();

    void PushSystems();

    void MenuBar();

    void DrawViewport();

    void DebugLighting();

    void Hide();

    void Show();

    void Quit();

    EditorMode m_mode{NONE};

    Ref<ScenePanel> m_scene_panel;
    Ref<EditorCameraSystem> m_editor_camera_system;
    Ref<CameraSystem> m_camera_system;
    Ref<LightingSystem> m_lighting_system;
    Ref<SkyboxSystem> m_skybox_system;
    Ref<SpriteRenderSystem> m_sprite_system;
    Ref<PostProcessSystem> m_post_process_system;
    Ref<ProfileSystem> m_profile_system;
    Ref<TileMapSystem> m_tile_map_system;

    int m_width;
    int m_height;
    int m_msaa;

    Ref<Framebuffer> m_debug_gbuffer;
    Ref<Framebuffer> m_screen_buffer;

    bool m_hide;
    bool m_quitting;

    bool m_load_scene_open;
    bool m_save_scene_open;
    ImFileDialogInfo m_file_dialog_info;

    Ref<Sprite> m_light_icon;

    Entity m_selected_entity;

    HandlerRegistration m_entity_select_handler;
};

}  // namespace SD

#endif /* EDITOR_LAYER_HPP */
