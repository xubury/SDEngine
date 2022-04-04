#ifndef SD_EDITOR_LAYER_HPP
#define SD_EDITOR_LAYER_HPP

#include "Renderer/GraphicsLayer.hpp"
#include "Core/Layer.hpp"
#include "Graphics/Camera.hpp"

#include "ScenePanel.hpp"
#include "EditorCameraSystem.hpp"
#include "AnimationEditor.hpp"
#include "TileMapSystem.hpp"

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

    void NewScene(SceneAsset* scene);
    void OpenLoadSceneDialog();
    void OpenSaveSceneDialog();

   private:
    void On(const KeyEvent& e) override;
    void On(const AppQuitEvent& e) override;
    void On(const MouseMotionEvent &e) override;

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

    Vector2i m_viewport_pos;

    Vector2i m_viewport_size;

    Ref<Framebuffer> m_viewport_target;
    Ref<Texture> m_scene_buffer;
    Ref<Texture> m_entity_buffer;

    bool m_is_runtime;
    bool m_quitting;

    bool m_load_scene_open;
    bool m_save_scene_open;
    ImFileDialogInfo m_file_dialog_info;

    SceneAsset* m_scene_asset;
    Entity m_selected_entity;

    HandlerRegistration m_entity_select_handler;
};

}  // namespace SD

#endif /* EDITOR_LAYER_HPP */
