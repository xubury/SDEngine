#ifndef SD_TILE_LAYOUT_HPP
#define SD_TILE_LAYOUT_HPP

#include "TileMap/Export.hpp"
#include "Renderer/Sprite.hpp"
#include "Graphics/Texture.hpp"

#include <unordered_map>

template <>
struct std::hash<glm::ivec2> {
    std::size_t operator()(const glm::ivec2 &vec) const {
        return hash<int>()(vec.x) ^ hash<int>()(vec.y);
    }
};

namespace SD {

class SD_TILE_MAP_API TileLayout {
   public:
    TileLayout();

    void SetSprite(const glm::ivec2 &pos, const Sprite &sprite);
    Sprite &GetSprite(const glm::ivec2 &pos);
    bool HasSprite(const glm::ivec2 &pos) const;
    void ClearSprite(const glm::ivec2 &pos);

    void SetTileSize(const glm::ivec2 &tile_size) { m_tile_size = tile_size; }
    const glm::ivec2 &GetTileSize() const { return m_tile_size; }

    const std::unordered_map<glm::ivec2, Sprite> &GetSprites() const {
        return m_sprites;
    }

    const Ref<Texture> GetOutlineTexture() const { return m_outline_texture; }

    glm::ivec2 MapWorldToTile(const glm::vec2 &world) const;
    glm::vec2 MapTileToWorld(const glm::ivec2 &tile) const;

   private:
    glm::ivec2 m_tile_size;
    std::unordered_map<glm::ivec2, Sprite> m_sprites;
    Ref<Texture> m_outline_texture;
};

}  // namespace SD

#endif  // !SD_TILE_LAYOUT_HPP
