#include "TileMap/TileLayout.hpp"

namespace SD {

const int DEFAULT_TILE_SIZE = 20;

const int GRID_TEXTURE_SIZE = 100;

const int LINE_WIDTH = 10;

TileLayout::TileLayout() : m_tile_size(DEFAULT_TILE_SIZE) {
    m_outline_texture =
        Texture::Create(GRID_TEXTURE_SIZE, GRID_TEXTURE_SIZE,
                        TextureSpec(1, TextureType::TEX_2D, DataFormat::RGBA,
                                    DataFormatType::UBYTE));
    size_t pixel_size =
        4 * m_outline_texture->GetWidth() * m_outline_texture->GetHeight();
    void *data = malloc(pixel_size);
    memset(data, 0xff, pixel_size);
    m_outline_texture->SetPixels(0, 0, 0, m_outline_texture->GetWidth(),
                                 m_outline_texture->GetHeight(), 1, data);
    memset(data, 0x00, pixel_size);
    m_outline_texture->SetPixels(
        LINE_WIDTH, LINE_WIDTH, 0,
        m_outline_texture->GetWidth() - LINE_WIDTH * 2,
        m_outline_texture->GetHeight() - LINE_WIDTH * 2, 1, data);

    free(data);
}

void TileLayout::SetSprite(const glm::ivec2 &pos, const Sprite &sprite) {
    if (HasSprite(pos)) {
        ClearSprite(pos);
    }
    m_sprites.emplace(pos, sprite);
}

Sprite &TileLayout::GetSprite(const glm::ivec2 &pos) {
    return m_sprites.at(pos);
}

bool TileLayout::HasSprite(const glm::ivec2 &pos) const {
    return m_sprites.count(pos);
}

void TileLayout::ClearSprite(const glm::ivec2 &pos) { m_sprites.erase(pos); }

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
