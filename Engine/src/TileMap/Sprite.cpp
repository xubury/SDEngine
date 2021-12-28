#include "TileMap/Sprite.hpp"

namespace SD {

Sprite::Sprite(const Ref<Texture> &texture, const std::array<glm::vec2, 2> &uvs)
    : m_texture(texture), m_uvs(uvs) {}

void Sprite::SetUVs(const std::array<glm::vec2, 2> &uvs) { m_uvs = uvs; }

}  // namespace SD
