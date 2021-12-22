#ifndef SD_TILE_MAP_SYSTEM_HPP
#define SD_TILE_MAP_SYSTEM_HPP

#include "System/Export.hpp"
#include "Core/System.hpp"
#include "TileMap/TileMap.hpp"
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

    void Add(const glm::vec2 &pos);

    glm::ivec2 MapWorldToTile(const glm::vec2 &world);
    glm::vec2 MapTileToWorld(const glm::ivec2 &tile);

   private:
    Ref<Texture> m_outline;

    std::vector<std::pair<glm::ivec2, Tile>> m_contents;

    TileMap m_map;
    Tile m_tile;

    bool m_fileDialogOpen;
    ImFileDialogInfo m_fileDialogInfo;
};

}  // namespace SD

#endif  // !SD_TILE_MAP_SYSTEM_HPP
