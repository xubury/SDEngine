#ifndef SD_EDITOR_LAYER_HPP
#define SD_EDITOR_LAYER_HPP

#include "Core/Layer.hpp"
#include "Graphics/Camera.hpp"

#include "ScenePanel.hpp"
#include "EditorCameraSystem.hpp"
#include "AnimationEditor.hpp"
#include "TileMapSystem.hpp"

#include "System/CameraSystem.hpp"
#include "System/LightingSystem.hpp"
#include "System/SkyboxSystem.hpp"
#include "System/ProfileSystem.hpp"
#include "System/PostProcessSystem.hpp"
#include "System/SpriteRenderSystem.hpp"

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

    void NewScene();
    void OpenLoadSceneDialog();
    void OpenSaveSceneDialog();

   private:
    void OnKeyEvent(const KeyEvent& e);
    void OnWindowSizeEvent(const WindowSizeEvent& e);
    void OnViewportUpdate();
    void BlitViewportBuffer();

    void InitBuffers();

    void ProcessDialog();

    void PushSystems();

    void MenuBar();

    void DrawViewport();

    void DebugLighting();

    void Hide();

    void Show();

    void Quit();

    EditorMode m_mode{NONE};

    ScenePanel* m_scene_panel;
    EditorCameraSystem* m_editor_camera_system;
    CameraSystem* m_camera_system;
    LightingSystem* m_lighting_system;
    SkyboxSystem* m_skybox_system;
    SpriteRenderSystem* m_sprite_system;
    PostProcessSystem* m_post_process_system;
    ProfileSystem* m_profile_system;
    TileMapSystem* m_tile_map_system;
    AnimationEditor* m_animation_editor;

    glm::ivec2 m_viewport_pos;
    bool m_viewport_pos_update;

    glm::ivec2 m_viewport_size;
    bool m_viewport_size_update;

    bool m_viewport_focus;
    bool m_viewport_hover;
    bool m_viewport_state_update;

    int m_msaa;

    Ref<Framebuffer> m_debug_gbuffer;
    Ref<Framebuffer> m_viewport_buffer;

    bool m_is_runtime;
    bool m_quitting;

    bool m_load_scene_open;
    bool m_save_scene_open;
    ImFileDialogInfo m_file_dialog_info;

    Ref<Texture> m_light_icon;

    Entity m_selected_entity;

    HandlerRegistration m_window_size_handler;
    HandlerRegistration m_key_handler;
    HandlerRegistration m_entity_select_handler;
};

}  // namespace SD

#endif /* EDITOR_LAYER_HPP */
