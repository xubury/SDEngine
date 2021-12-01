#ifndef SD_SKYBOX_HPP
#define SD_SKYBOX_HPP

#include "Utility/Base.hpp"
#include "Renderer/Export.hpp"

#include "Graphics/Texture.hpp"
#include "Asset/Image.hpp"

namespace SD {

class SD_RENDERER_API Skybox {
   public:
    Skybox();

    void SetFace(CubeMapFace face, Image &image);
    Texture *GetTexture() { return m_texture.get(); }

    bool Valid() const;
    bool Valid(CubeMapFace face) const;

   private:
    Ref<Texture> m_texture;
    uint32_t m_width;
    uint32_t m_height;
    TextureFormat m_format;
    uint8_t m_valid_mask;
};

}  // namespace SD

#endif  // ! SD_SKYBOX_HPP
