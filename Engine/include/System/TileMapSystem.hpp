#ifndef SD_TILE_MAP_SYSTEM_HPP
#define SD_TILE_MAP_SYSTEM_HPP

#include "System/Export.hpp"
#include "Core/System.hpp"
#include "TileMap/TileMap.hpp"
#include "TileMap/TileLayout.hpp"
#include "TileMap/Tile.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "ImGui/FileDialog.hpp"

namespace SD {

class SD_SYSTEM_API TileMapSystem : public System {
   public:
    TileMapSystem();

    void OnTick(float dt) override;

    void OnPush() override;
    void OnPop() override;

    void OnImGui() override;
    void OnRender() override;

    void AddSelectTileToWorld();

    void SetActivePos(const glm::vec2 &world);

   private:
    TileLayout m_layout;

    TileMap m_map;
    Tile m_selected_tile;
    bool m_has_select{false};

    glm::ivec2 m_active_tile_pos;

    bool m_fileDialogOpen;
    ImFileDialogInfo m_fileDialogInfo;
};

}  // namespace SD

#endif  // !SD_TILE_MAP_SYSTEM_HPP
