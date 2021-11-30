#include "Renderer/Skybox.hpp"

namespace SD {

Skybox::Skybox() : m_valid_mask(0) {}

void Skybox::SetFace(CubeMapFace face, Image &image) {
    TextureFormat format =
        image.HasAlpha() ? TextureFormat::RGBA : TextureFormat::RGB;
    uint8_t mask = GetCubeMapFaceMask(face);
    if (m_texture == nullptr || m_width != image.Width() ||
        m_height != image.Height() || format != m_format) {
        m_width = image.Width();
        m_height = image.Height();
        m_format = format;
        m_texture = Texture::Create(m_width, m_height, 1, TextureType::TEX_CUBE,
                                    m_format, TextureFormatType::UBYTE,
                                    TextureWrap::EDGE, TextureFilter::LINEAR,
                                    TextureMipmapFilter::LINEAR);
    }
    m_texture->SetPixels(image.Width(), image.Height(), image.Data(), mask);
    m_valid_mask |= mask;
}

bool Skybox::Valid() const {
    return m_valid_mask == CubeMapFaceMask::CUBE_MAP_FACE_ALL;
}

bool Skybox::Valid(CubeMapFace face) const {
    uint8_t mask = GetCubeMapFaceMask(face);
    return m_valid_mask & mask;
}

}  // namespace SD
