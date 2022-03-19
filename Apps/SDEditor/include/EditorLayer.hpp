#ifndef SD_EDITOR_LAYER_HPP
#define SD_EDITOR_LAYER_HPP

#include "Renderer/GraphicsLayer.hpp"
#include "Core/Layer.hpp"
#include "Graphics/Camera.hpp"

#include "ScenePanel.hpp"
#include "EditorCameraSystem.hpp"
#include "AnimationEditor.hpp"
#include "TileMapSystem.hpp"
#include "ProfileSystem.hpp"

#include "Asset/SceneAsset.hpp"

namespace SD {

enum class EditorMode { None, TwoDimensional, ThreeDimensional };
class EditorLayer : public Layer {
   public:
    EditorLayer(GraphicsLayer* graphics_layer, int width, int height);
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

    void InitBuffers();

    void ProcessDialog();

    void MenuBar();

    void DrawViewport();

    void DrawDebugBuffers();

    void Quit();

    EditorMode m_mode;
    GraphicsLayer* m_graphics_layer;

    ScenePanel* m_scene_panel;
    EditorCameraSystem* m_editor_camera_system;
    TileMapSystem* m_tile_map_system{nullptr};
    AnimationEditor* m_animation_editor;

    glm::ivec2 m_viewport_pos;

    glm::ivec2 m_viewport_size;
    bool m_viewport_size_update;

    Ref<Framebuffer> m_viewport_buffer;

    bool m_is_runtime;
    bool m_quitting;

    bool m_load_scene_open;
    bool m_save_scene_open;
    ImFileDialogInfo m_file_dialog_info;

    SceneAsset* m_scene_asset;
    Entity m_selected_entity;

    HandlerRegistration m_window_size_handler;
    HandlerRegistration m_key_handler;
    HandlerRegistration m_entity_select_handler;
};

}  // namespace SD

#endif /* EDITOR_LAYER_HPP */
