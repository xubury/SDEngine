#ifndef SD_EDITOR_LAYER_HPP
#define SD_EDITOR_LAYER_HPP

#include "Core/GraphicsLayer.hpp"
#include "Core/Layer.hpp"
#include "Graphics/Camera.hpp"
#include "ECS/SceneManager.hpp"

#include "ScenePanel.hpp"
#include "EditorCamera.hpp"
#include "AnimationEditor.hpp"
#include "TileMapEditor.hpp"
#include "ContentBrowser.hpp"

namespace SD {

enum class EditorMode { None, TwoDimensional, ThreeDimensional };

class EditorLayer : public Layer {
   public:
    EditorLayer(InputDevice* input, SceneManager* scenes,
                ResourceManager* resources, GraphicsLayer* graphics_layer,
                int width, int height);
    ~EditorLayer();

    void OnInit() override;

    void OnTick(float dt) override;

    void OnRender() override;

    void OnImGui() override;

    void OpenLoadSceneDialog();
    void OpenSaveSceneDialog();

   private:
    void On(const KeyEvent& e) override;
    void On(const AppQuitEvent& e) override;
    void On(const MouseMotionEvent& e) override;

    void InitBuffers();

    void ProcessDialog();

    void MenuBar();

    void DrawViewport();

    void DrawDebugBuffers();

    void Quit();

    EditorMode m_mode;

    SceneManager* m_scenes;
    ResourceManager* m_resources;

    GraphicsLayer* m_graphics_layer;

    EventDispatcher m_dispatcher;
    ScenePanel m_scene_panel;
    EditorCamera m_editor_camera;
    TileMapEditor m_tile_map_editor;
    AnimationEditor m_animation_editor;
    ContentBrowser m_content_browser;

    Vector2i m_viewport_pos;

    Vector2i m_viewport_size;

    Ref<Framebuffer> m_viewport_target;
    Ref<Texture> m_scene_buffer;

    bool m_is_runtime;
    bool m_quitting;

    bool m_load_scene_open;
    bool m_save_scene_open;
    ImFileDialogInfo m_file_dialog_info;

    Entity m_selected_entity;

    HandlerRegistration m_entity_select_handler;
};

}  // namespace SD

#endif /* EDITOR_LAYER_HPP */
