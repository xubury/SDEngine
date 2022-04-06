#ifndef SD_TILE_MAP_SYSTEM_HPP
#define SD_TILE_MAP_SYSTEM_HPP

#include "ECS/Component.hpp"
#include "Graphics/Viewport.hpp"
#include "Graphics/Framebuffer.hpp"
#include "Graphics/Camera.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "ImGui/FileDialog.hpp"

#include "Asset/TextureAsset.hpp"

#include "TileBrush.hpp"

namespace SD {

class TileMapEditor {
   public:
    enum class Operation { None, AddEntity, RemoveEntity };

   public:
    TileMapEditor();
    void ImGui();
    void Render();

    bool ManipulateScene(Camera &camera, Vector3f &world);
    const SpriteFrame &GetSpriteFrame() const { return m_frame; }

   private:
    void SetViewport(float left, float top, float width, float height);
    Ref<Texture> m_outline_texture;

    TileBrush m_brush;

    // Selected sprite and its uvs
    SpriteFrame m_frame;

    bool m_draw_outline;

    Operation m_operation;

    Viewport m_viewport;
};

}  // namespace SD

#endif  // !SD_TILE_MAP_SYSTEM_HPP
