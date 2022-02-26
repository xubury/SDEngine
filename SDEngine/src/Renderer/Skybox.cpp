#include "Renderer/Skybox.hpp"

namespace SD {

Skybox::Skybox() {}

void Skybox::SetFace(CubeMapFace face, int width, int height, int channels,
                     int bits_per_pixels, const void *data) {
    DataFormat format = GetDataFormat(channels);
    DataFormatType format_type = GetDataFormatType(bits_per_pixels, channels);
    if (m_texture == nullptr || m_width != width || m_height != height ||
        format != m_format || format_type != m_format_type) {
        m_width = width;
        m_height = height;
        m_format = format;
        m_format_type = format_type;
        m_texture =
            Texture::Create(m_width, m_height,
                            TextureSpec(1, TextureType::TEX_CUBE, m_format,
                                        m_format_type, TextureWrap::EDGE));
    }
    m_texture->SetPixels(0, 0, static_cast<int>(face), m_width, m_height, 1,
                         data);
}

}  // namespace SD
