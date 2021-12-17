#ifndef SD_TILE_MAP_HPP
#define SD_TILE_MAP_HPP

#include "TileMap/Export.hpp"
#include "TileMap/Tile.hpp"

#include "Graphics/Texture.hpp"

#include <glm/glm.hpp>
#include <vector>

namespace SD {

class SD_TILE_MAP_API TileMap {
   public:
    TileMap(const glm::ivec2 &size, const glm::ivec2 &grid_size);

    Ref<Texture> GetTexture() { return m_map; }

    Ref<Texture> GetOutline() { return m_outline; }

    void Set(const glm::ivec2 &pos, const Tile &tile);

    const Tile &Get(const glm::ivec2 &pos) const;

    Tile &At(const glm::ivec2 &pos);

   private:
    glm::ivec2 m_size;
    glm::ivec2 m_grid_size;

    std::vector<Tile> m_tiles;

    Ref<Texture> m_map;
    Ref<Texture> m_outline;
};

};  // namespace SD

#endif  // !SD_TILE_MAP_HPP
