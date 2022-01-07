#ifndef SD_SKYBOX_HPP
#define SD_SKYBOX_HPP

#include "Utility/Base.hpp"
#include "Renderer/Export.hpp"

#include "Graphics/Texture.hpp"
#include "Renderer/Bitmap.hpp"

namespace SD {

class SD_RENDERER_API Skybox {
   public:
    Skybox();

    void SetFace(CubeMapFace face, const Bitmap &bitmap);
    Texture *GetTexture() { return m_texture.get(); }

    bool Valid() const;
    bool Valid(CubeMapFace face) const;

   private:
    Ref<Texture> m_texture;
    uint32_t m_width;
    uint32_t m_height;
    DataFormat m_format;
};

}  // namespace SD

#endif  // ! SD_SKYBOX_HPP