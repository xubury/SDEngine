#include "TileMap/TileMap.hpp"

namespace SD {

TileMap::TileMap(const glm::ivec2 &grid_cnt, float grid_size)
    : m_grid_cnt(grid_cnt), m_grid_size(grid_size) {
    glm::vec2 size = glm::vec2(m_grid_cnt) * m_grid_size;
    m_center = size / 2.f;
    void *data = malloc(size.x * size.y * 4);
    memset(data, 0xff, size.x * size.y * 4);
    m_map =
        Texture::Create(size.x, size.y,
                        TextureSpec(1, TextureType::TEX_2D, DataFormat::RGBA,
                                    DataFormatType::UBYTE));
    SD_TRACE("center:{}, size:{}", m_center, size);
    m_map->SetPixels(0, 0, 0, size.x, size.y, 1, data);
    free(data);

    int outline_thickness = std::max<int>(0.05f * grid_size, 1);
    data = malloc(m_grid_size * m_grid_size * 4);
    memset(data, 0x77, m_grid_size * m_grid_size * 4);
    m_outline =
        Texture::Create(m_grid_size, m_grid_size,
                        TextureSpec(1, TextureType::TEX_2D, DataFormat::RGBA,
                                    DataFormatType::UBYTE));
    m_outline->SetPixels(0, 0, 0, m_grid_size, m_grid_size, 1, data);
    memset(data, 0x00, m_grid_size * m_grid_size * 4);
    m_outline->SetPixels(outline_thickness, outline_thickness, 0,
                         m_grid_size - outline_thickness * 2,
                         m_grid_size - outline_thickness * 2, 1, data);

    free(data);
}

void TileMap::Set(const glm::ivec2 &pos, const Tile &tile) {
    glm::vec2 top_left = MapTileToWorld(pos);
    top_left.x = top_left.x + m_center.x;
    top_left.y = -top_left.y + m_center.y;

    At(pos) = tile;

    void *data = malloc(m_grid_size * m_grid_size * 4);
    memset(data, 0x00, m_grid_size * m_grid_size * 4);
    m_map->SetPixels(top_left.x, top_left.y, 0, m_grid_size, m_grid_size, 1,
                     data);
    free(data);
}

const Tile &TileMap::Get(const glm::ivec2 &pos) const {
    return m_tiles[pos.x + pos.y * m_grid_cnt.x];
}

Tile &TileMap::At(const glm::ivec2 &pos) {
    return m_tiles[pos.x + pos.y * m_grid_cnt.x];
}

glm::ivec2 TileMap::MapWorldToTile(const glm::vec2 &world) {
    glm::vec2 tile;
    tile.x = (world.x + m_center.x) / m_grid_size;
    tile.y = (m_center.y - world.y) / m_grid_size;
    return tile;
}

glm::vec2 TileMap::MapTileToWorld(const glm::ivec2 &tile) {
    glm::vec2 world;
    world.x = tile.x * m_grid_size - m_center.x;
    world.y = m_center.y - tile.y * m_grid_size;
    return world;
}

}  // namespace SD
