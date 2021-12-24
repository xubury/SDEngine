#include "TileMap/TileMap.hpp"

namespace SD {

void TileMap::SetTexture(const Ref<Texture> &texture) { m_map = texture; }

std::array<glm::vec2, 2> TileMap::GetTileUVs(
    const glm::vec2 &texture_pos) const {
    std::array<glm::vec2, 2> uv;
    uv[0] = glm::floor(texture_pos / glm::vec2(m_tile_size));
    uv[1] = uv[0] + glm::vec2(1.f);

    uv[0].x = uv[0].x * m_tile_size.x / m_map->GetWidth();
    uv[0].y = uv[0].y * m_tile_size.y / m_map->GetHeight();

    uv[1].x = uv[1].x * m_tile_size.x / m_map->GetWidth();
    uv[1].y = uv[1].y * m_tile_size.y / m_map->GetHeight();
    return uv;
}

}  // namespace SD
