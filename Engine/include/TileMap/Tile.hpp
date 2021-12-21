#ifndef SD_TILE_HPP
#define SD_TILE_HPP

#include "TileMap/Export.hpp"
#include "Graphics/Texture.hpp"

namespace SD {

class SD_TILE_MAP_API Tile {
   public:
    Tile() = default;

    Ref<Texture> m_texture;
   private:
};

}  // namespace SD

#endif  // !SD_TILE_HPP
