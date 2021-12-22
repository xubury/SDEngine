#include "TileMap/Tile.hpp"

namespace SD {

void Tile::Set(const TileMap &map, const glm::vec2 &pos) {
    m_texture = map.GetTexture();

    int tile_size = map.GetTileSize();
    m_uv[0] = glm::floor(pos / static_cast<float>(tile_size));
    m_uv[1] = m_uv[0] + glm::vec2(1.f);

    m_uv[0].x = m_uv[0].x * tile_size / m_texture->GetWidth();
    m_uv[0].y = m_uv[0].y * tile_size / m_texture->GetHeight();

    m_uv[1].x = m_uv[1].x * tile_size / m_texture->GetWidth();
    m_uv[1].y = m_uv[1].y * tile_size / m_texture->GetHeight();
}

std::array<glm::vec2, 2> Tile::GetTileMapUV() const { return m_uv; }

}  // namespace SD
