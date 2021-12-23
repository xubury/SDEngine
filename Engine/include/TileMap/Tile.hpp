#ifndef SD_TILE_HPP
#define SD_TILE_HPP

#include "TileMap/Export.hpp"
#include "Graphics/Texture.hpp"

namespace SD {

class SD_TILE_MAP_API Tile {
   public:
    Tile() = default;
    Tile(const Ref<Texture> &texture, const std::array<glm::vec2, 2> &uvs);

    void Set(const Ref<Texture> &texture, const std::array<glm::vec2, 2> &uvs);

    std::array<glm::vec2, 2> GetUVs() const { return m_uvs; }

    const Ref<Texture> GetTexture() const { return m_texture; }

   private:
    Ref<Texture> m_texture;
    std::array<glm::vec2, 2> m_uvs;
};

}  // namespace SD

#endif  // !SD_TILE_HPP
