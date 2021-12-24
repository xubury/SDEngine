#ifndef SD_SPRITE_HPP
#define SD_SPRITE_HPP

#include "Renderer/Export.hpp"
#include "Graphics/Texture.hpp"

namespace SD {

class SD_RENDERER_API Sprite {
   public:
    Sprite() = default;
    Sprite(const Ref<Texture> &texture, const std::array<glm::vec2, 2> &uvs);

    void Set(const Ref<Texture> &texture, const std::array<glm::vec2, 2> &uvs);

    void SetTexture(const Ref<Texture> &texture) { m_texture = texture; }

    const std::array<glm::vec2, 2> &GetUVs() const { return m_uvs; }

    const Ref<Texture> GetTexture() const { return m_texture; }

    bool Valid() const { return m_texture != nullptr; }

   private:
    Ref<Texture> m_texture;
    std::array<glm::vec2, 2> m_uvs;
};

}  // namespace SD

#endif  // !SD_SPRITE_HPP
