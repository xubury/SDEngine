#ifndef SD_SKYBOX_HPP
#define SD_SKYBOX_HPP

#include "Utility/Base.hpp"
#include "Renderer/Export.hpp"

#include "Graphics/Texture.hpp"

namespace SD {

class SD_RENDERER_API Skybox {
   public:
    Skybox();

    void SetFace(CubeMapFace face, int width, int height, int channels,
                 int bits_per_pixels, const void *data);
    Texture *GetTexture() { return m_texture.get(); }

    bool Valid() const;
    bool Valid(CubeMapFace face) const;

   private:
    Ref<Texture> m_texture;
    int m_width;
    int m_height;
    DataFormat m_format;
    DataFormatType m_format_type;
};

}  // namespace SD

#endif  // ! SD_SKYBOX_HPP
