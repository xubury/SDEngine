#ifndef SD_SPRITE_HPP
#define SD_SPRITE_HPP

#include "Renderer/Export.hpp"
#include "Graphics/Texture.hpp"
#include "Utility/Serialize.hpp"

namespace SD {

class SD_RENDERER_API Sprite {
   public:
    Sprite(int width, int height, const TextureSpec &spec) {
        m_texture = Texture::Create(width, height, spec);
    }
    Sprite(const Ref<Texture> &texture);

    void SetTexture(const Ref<Texture> &texture) { m_texture = texture; }

    const Ref<Texture> GetTexture() const { return m_texture; }

    Ref<Texture> GetTexture() { return m_texture; }

   private:
    Ref<Texture> m_texture;
};

}  // namespace SD

#endif  // !SD_SPRITE_HPP
