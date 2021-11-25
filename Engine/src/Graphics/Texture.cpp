#include "Graphics/Texture.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Graphics/OpenGL/GLTexture.hpp"

namespace SD {

Ref<Texture> Texture::Create(int width, int height, int samples,
                             TextureType type, TextureFormat format,
                             TextureFormatType format_type, TextureWrap wrap,
                             TextureFilter filter,
                             TextureMipmapFilter mipmap_filter,
                             const void *data) {
    Ref<Texture> texture;
    switch (GetGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            texture = CreateRef<GLTexture>(width, height, samples, type, format,
                                           format_type, wrap, filter,
                                           mipmap_filter, data);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return texture;
}

Texture::Texture(int width, int height, int samples, TextureType type,
                 TextureFormat format, TextureFormatType format_type,
                 TextureWrap wrap, TextureFilter filter,
                 TextureMipmapFilter mipmap_filter)
    : m_width(width),
      m_height(height),
      m_samples(samples),
      m_type(type),
      m_format(format),
      m_format_type(format_type),
      m_wrap(wrap),
      m_filter(filter),
      m_mipmap_filter(mipmap_filter) {}

bool Texture::operator==(const Texture &other) const {
    return GetId() == other.GetId();
}

bool Texture::operator!=(const Texture &other) const {
    return !(*this == other);
}

int Texture::GetWidth() const { return m_width; }

int Texture::GetHeight() const { return m_height; }

int Texture::GetSamples() const { return m_samples; }

TextureType Texture::GetType() const { return m_type; }

TextureFormat Texture::GetFormat() const { return m_format; }

TextureFormatType Texture::GetFormatType() const { return m_format_type; }

}  // namespace SD
