#include "Renderer/Skybox.hpp"

namespace SD {

Skybox::Skybox() : m_valid_mask(0) {}

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
    m_texture->SetPixels(0, 0, static_cast<size_t>(face), m_width, m_height, 1,
                         bitmap.Data());
    m_valid_mask |= GetCubeMapFaceMask(face);
}

bool Skybox::Valid() const {
    return m_valid_mask == CubeMapFaceMask::CUBE_MAP_FACE_ALL;
}

bool Skybox::Valid(CubeMapFace face) const {
    uint8_t mask = GetCubeMapFaceMask(face);
    return m_valid_mask & mask;
}

}  // namespace SD
