#include "TileMap/TileMap.hpp"

namespace SD {

TileMap::TileMap(const glm::ivec2 &size, const glm::ivec2 &grid_size)
    : m_size(size), m_grid_size(grid_size) {
    void *data = malloc(m_size.x * m_size.y * 4);
    memset(data, 0xff, m_size.x * m_size.y * 4);
    m_map =
        Texture::Create(size.x, size.y,
                        TextureSpec(1, TextureType::TEX_2D, DataFormat::RGBA,
                                    DataFormatType::UBYTE));
    m_map->SetPixels(0, 0, 0, m_size.x, m_size.y, 1, data);
    free(data);

    int outline_thickness = 0.05f * std::min(grid_size.x, grid_size.y);
    data = malloc(m_grid_size.x * m_grid_size.y * 4);
    memset(data, 0x77, m_grid_size.x * m_grid_size.y * 4);
    m_outline =
        Texture::Create(m_grid_size.x, m_grid_size.y,
                        TextureSpec(1, TextureType::TEX_2D, DataFormat::RGBA,
                                    DataFormatType::UBYTE));
    m_outline->SetPixels(0, 0, 0, m_grid_size.x, m_grid_size.y, 1, data);
    memset(data, 0x00, m_grid_size.x * m_grid_size.y * 4);
    m_outline->SetPixels(outline_thickness, outline_thickness, 0,
                         m_grid_size.x - outline_thickness * 2,
                         m_grid_size.y - outline_thickness * 2, 1, data);

    free(data);
}

void TileMap::Set(const glm::ivec2 &pos, const Tile &tile) {
    glm::ivec2 top_left = pos * m_grid_size;
    SD_CORE_ASSERT(top_left.x >= 0 && top_left.x < m_size.x, "size invalid");
    SD_CORE_ASSERT(top_left.y >= 0 && top_left.y < m_size.y, "size invalid");

    At(pos) = tile;

    void *data = malloc(m_grid_size.x * m_grid_size.y * 4);
    memset(data, 0x00, m_grid_size.x * m_grid_size.y * 4);
    m_map->SetPixels(top_left.x, top_left.y, 0, m_grid_size.x, m_grid_size.y, 1,
                     data);
    free(data);
}

const Tile &TileMap::Get(const glm::ivec2 &pos) const {
    return m_tiles[pos.x + pos.y * m_size.x / m_grid_size.x];
}

Tile &TileMap::At(const glm::ivec2 &pos) {
    return m_tiles[pos.x + pos.y * m_size.x / m_grid_size.x];
}

}  // namespace SD
