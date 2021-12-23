#include "TileMap/Tile.hpp"

namespace SD {

Tile::Tile(const Ref<Texture> &texture, const std::array<glm::vec2, 2> &uvs)
    : m_texture(texture), m_uvs(uvs) {}

void Tile::Set(const Ref<Texture> &texture,
               const std::array<glm::vec2, 2> &uvs) {
    m_texture = texture;
    m_uvs = uvs;
}

}  // namespace SD
