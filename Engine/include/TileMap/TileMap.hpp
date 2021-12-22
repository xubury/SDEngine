#ifndef SD_TILE_MAP_HPP
#define SD_TILE_MAP_HPP

#include "TileMap/Export.hpp"

#include "Renderer/Bitmap.hpp"
#include "Graphics/Texture.hpp"

#include <glm/glm.hpp>
#include <vector>

namespace SD {

class SD_TILE_MAP_API TileMap {
   public:
    TileMap() = default;

    void SetTileSize(int tile_size) { m_tile_size = tile_size; }
    int GetTileSize() const { return m_tile_size; }

    void SetTileMap(const Bitmap &bitmap);
    const Ref<Texture> GetTexture() const { return m_map; }

   private:
    int m_tile_size{50};

    Ref<Texture> m_map;
};

};  // namespace SD

#endif  // !SD_TILE_MAP_HPP
