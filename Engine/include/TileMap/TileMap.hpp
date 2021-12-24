#ifndef SD_TILE_MAP_HPP
#define SD_TILE_MAP_HPP

#include "TileMap/Export.hpp"

#include "Renderer/Bitmap.hpp"
#include "Graphics/Texture.hpp"

namespace SD {

class SD_TILE_MAP_API TileMap {
   public:
    TileMap() = default;

    void SetTileSize(const glm::ivec2 &tile_size) { m_tile_size = tile_size; }
    glm::ivec2 GetTileSize() const { return m_tile_size; }

    void SetTexture(const Ref<Texture> &texture);

    const Ref<Texture> GetTexture() const { return m_map; }

    std::array<glm::vec2, 2> GetTileUVs(const glm::vec2 &texture_pos) const;

   private:
    glm::ivec2 m_tile_size{50, 50};

    Ref<Texture> m_map;
};

};  // namespace SD

#endif  // !SD_TILE_MAP_HPP
