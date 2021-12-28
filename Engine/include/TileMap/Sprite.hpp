#ifndef SD_SPRITE_HPP
#define SD_SPRITE_HPP

#include "TileMap/Export.hpp"
#include "Graphics/Texture.hpp"
#include "Utility/Serialize.hpp"

namespace SD {

class SD_TILE_MAP_API Sprite {
   public:
    Sprite() = default;
    Sprite(const Ref<Texture> &texture, const std::array<glm::vec2, 2> &uvs);

    void SetUVs(const std::array<glm::vec2, 2> &uvs);

    void SetTexture(const Ref<Texture> &texture) { m_texture = texture; }

    const std::array<glm::vec2, 2> &GetUVs() const { return m_uvs; }

    const Ref<Texture> GetTexture() const { return m_texture; }

    SERIALIZE(m_uvs)
   private:
    Ref<Texture> m_texture;
    std::array<glm::vec2, 2> m_uvs;
};

}  // namespace SD

#endif  // !SD_SPRITE_HPP
