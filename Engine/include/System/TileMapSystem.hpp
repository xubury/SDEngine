#ifndef SD_TILE_MAP_SYSTEM_HPP
#define SD_TILE_MAP_SYSTEM_HPP

#include "System/Export.hpp"
#include "Core/System.hpp"
#include "TileMap/TileMap.hpp"
#include "TileMap/TileLayout.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "ImGui/FileDialog.hpp"

namespace SD {

class SD_SYSTEM_API TileMapSystem : public System {
    enum Operation { ADD_SPRITE, CLEAR_SPRITE };

   public:
    TileMapSystem();

    void OnTick(float dt) override;

    void OnPush() override;
    void OnPop() override;

    void OnImGui() override;
    void OnRender() override;

    void SelectWorldPos(const glm::vec2 &world);

    void ApplyActionAtPos();

   private:
    void RenderOutline();

    TileLayout m_layout;

    TileMap m_map;
    ResourceId m_map_id;

    Sprite m_selected_sprite;

    glm::ivec2 m_select_tile_pos;

    bool m_fileDialogOpen;
    ImFileDialogInfo m_fileDialogInfo;

    bool m_draw_outline{true};

    Operation m_operation;
};

}  // namespace SD

#endif  // !SD_TILE_MAP_SYSTEM_HPP
