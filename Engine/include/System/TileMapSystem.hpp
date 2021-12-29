#ifndef SD_TILE_MAP_SYSTEM_HPP
#define SD_TILE_MAP_SYSTEM_HPP

#include "System/Export.hpp"
#include "Core/System.hpp"
#include "TileMap/Tile.hpp"
#include "Renderer/Sprite.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "ImGui/FileDialog.hpp"

namespace SD {

class SD_SYSTEM_API TileMapSystem : public System {
    enum Operation { ADD_ENTITY, REMOVE_ENTITY };

   public:
    TileMapSystem();

    void OnTick(float dt) override;

    void OnPush() override;
    void OnPop() override;

    void OnImGui() override;
    void OnRender() override;

    void SetViewport(const Viewport *viewport) { m_viewport = viewport; }

   private:
    const Viewport *m_viewport;
    void RenderOutline();

    Ref<Texture> m_outline_texture;

    ResourceId m_sprite_id;
    Tile m_brush;

    glm::ivec2 m_select_tile_pos;

    bool m_file_dialog_open;
    ImFileDialogInfo m_fileDialogInfo;

    bool m_draw_outline;

    Operation m_operation;
};

}  // namespace SD

#endif  // !SD_TILE_MAP_SYSTEM_HPP
