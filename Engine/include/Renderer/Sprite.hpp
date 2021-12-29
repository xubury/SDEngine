#ifndef SD_SPRITE_HPP
#define SD_SPRITE_HPP

#include "Renderer/Export.hpp"
#include "Graphics/Texture.hpp"
#include "Utility/Serialize.hpp"

namespace SD {

class SD_RENDERER_API Sprite {
   public:
    Sprite()
        : m_texture(Texture::Create(
              1, 1,
              TextureSpec(1, TextureType::TEX_2D, DataFormat::RGBA,
                          DataFormatType::UBYTE))){};
    Sprite(const Ref<Texture> &texture);

    void SetTexture(const Ref<Texture> &texture) { m_texture = texture; }

    const Ref<Texture> GetTexture() const { return m_texture; }

   private:
    Ref<Texture> m_texture;
};

}  // namespace SD

#endif  // !SD_SPRITE_HPP
