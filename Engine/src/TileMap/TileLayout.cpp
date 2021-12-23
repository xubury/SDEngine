#include "TileMap/TileLayout.hpp"

namespace SD {

const int DEFAULT_TILE_SIZE = 20;

const int GRID_TEXTURE_SIZE = 100;

const int LINE_WIDTH = 10;

TileLayout::TileLayout() : m_tile_size(DEFAULT_TILE_SIZE) {
    m_grid_texture =
        Texture::Create(GRID_TEXTURE_SIZE, GRID_TEXTURE_SIZE,
                        TextureSpec(1, TextureType::TEX_2D, DataFormat::RGBA,
                                    DataFormatType::UBYTE));
    size_t pixel_size =
        4 * m_grid_texture->GetWidth() * m_grid_texture->GetHeight();
    void *data = malloc(pixel_size);
    memset(data, 0xff, pixel_size);
    m_grid_texture->SetPixels(0, 0, 0, m_grid_texture->GetWidth(),
                              m_grid_texture->GetHeight(), 1, data);
    memset(data, 0x00, pixel_size);
    m_grid_texture->SetPixels(
        LINE_WIDTH, LINE_WIDTH, 0, m_grid_texture->GetWidth() - LINE_WIDTH * 2,
        m_grid_texture->GetHeight() - LINE_WIDTH * 2, 1, data);

    free(data);
}

void TileLayout::Set(const glm::ivec2 &pos, const Tile &tile) {
    if (HasTile(pos)) {
        Clear(pos);
    }
    m_tiles.emplace(pos, tile);
}

Tile &TileLayout::Get(const glm::ivec2 &pos) { return m_tiles.at(pos); }

bool TileLayout::HasTile(const glm::ivec2 &pos) const {
    return m_tiles.count(pos);
}

void TileLayout::Clear(const glm::ivec2 &pos) { m_tiles.erase(pos); }

glm::ivec2 TileLayout::MapWorldToTile(const glm::vec2 &world) const {
    glm::vec2 tile;
    tile.x = std::ceil(world.x / m_tile_size.x - 0.5f);
    tile.y = std::ceil(world.y / m_tile_size.y - 0.5f);
    return tile;
}

glm::vec2 TileLayout::MapTileToWorld(const glm::ivec2 &tile) const {
    glm::vec2 world;
    world.x = tile.x * m_tile_size.x;
    world.y = tile.y * m_tile_size.y;
    return world;
}

}  // namespace SD
