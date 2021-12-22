#ifndef SD_TILE_HPP
#define SD_TILE_HPP

#include "TileMap/Export.hpp"
#include "TileMap/TileMap.hpp"

namespace SD {

class SD_TILE_MAP_API Tile {
   public:
    Tile() : m_texture(nullptr){};

    bool Valid() const { return m_texture != nullptr; }
    void Set(const TileMap &map, const glm::vec2 &pos);

    std::array<glm::vec2, 2> GetTileMapUV() const;

    const Ref<Texture> GetTileMapTexture() const { return m_texture; }

   private:
    Ref<Texture> m_texture;
    std::array<glm::vec2, 2> m_uv;
};

}  // namespace SD

#endif  // !SD_TILE_HPP
