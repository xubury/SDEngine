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

    const Texture *GetTexture() const { return m_texture.get(); }

    Texture *GetTexture() { return m_texture.get(); }

   private:
    Ref<Texture> m_texture;
};

}  // namespace SD

#endif  // !SD_SPRITE_HPP
