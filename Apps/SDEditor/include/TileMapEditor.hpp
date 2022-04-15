#ifndef SD_TILE_MAP_SYSTEM_HPP
#define SD_TILE_MAP_SYSTEM_HPP

#include "ECS/Component.hpp"
#include "Graphics/Viewport.hpp"
#include "Graphics/Framebuffer.hpp"
#include "Graphics/Camera.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "ImGui/FileDialog.hpp"

#include "TileBrush.hpp"

namespace SD {

class TileMapEditor {
   public:
    enum class Operation { None, AddEntity, RemoveEntity };

   public:
    TileMapEditor();
    void ImGui();

    bool ManipulateScene(Scene &scene, const Camera &camera);
    const SpriteFrame &GetSpriteFrame() const { return m_frame; }

    const TileBrush &GetBrush() const { return m_brush; }

   private:
    void SetViewport(float left, float top, float width, float height);
    Scene *m_scene;

    TileBrush m_brush;

    // Selected sprite and its uvs
    SpriteFrame m_frame;

    Operation m_operation;

    Viewport m_viewport;
};

}  // namespace SD

#endif  // !SD_TILE_MAP_SYSTEM_HPP
