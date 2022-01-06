#include "Renderer/Skybox.hpp"

namespace SD {

Skybox::Skybox() {}

void Skybox::SetFace(CubeMapFace face, const Bitmap &bitmap) {
    DataFormat format = bitmap.HasAlpha() ? DataFormat::RGBA : DataFormat::RGB;
    if (m_texture == nullptr || m_width != bitmap.Width() ||
        m_height != bitmap.Height() || format != m_format) {
        m_width = bitmap.Width();
        m_height = bitmap.Height();
        m_format = format;
        m_texture = Texture::Create(
            m_width, m_height,
            TextureSpec(1, TextureType::TEX_CUBE, m_format,
                        DataFormatType::UBYTE, TextureWrap::EDGE));
    }
    m_texture->SetPixels(0, 0, static_cast<int>(face), m_width, m_height, 1,
                         bitmap.Data());
}

}  // namespace SD
