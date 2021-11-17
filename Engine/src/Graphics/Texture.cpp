#include "Graphics/Texture.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Graphics/OpenGL/GLTexture.hpp"

namespace SD {

Ref<Texture> Texture::Create(int width, int height, int samples,
                             TextureType type, TextureFormat format,
                             TextureFormatType formatType, TextureWrap wrap,
                             TextureFilter filter,
                             TextureMipmapFilter mipmapFilter,
                             const void *data) {
    Ref<Texture> texture;
    switch (GetGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            texture = CreateRef<GLTexture>(width, height, samples, type, format,
                                           formatType, wrap, filter,
                                           mipmapFilter, data);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return texture;
}

Texture::Texture(int width, int height, int samples, TextureType type,
                 TextureFormat format, TextureFormatType formatType,
                 TextureWrap wrap, TextureFilter filter,
                 TextureMipmapFilter mipmapFilter)
    : m_width(width),
      m_height(height),
      m_samples(samples),
      m_type(type),
      m_format(format),
      m_formatType(formatType),
      m_wrap(wrap),
      m_filter(filter),
      m_mipmapFilter(mipmapFilter) {}

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

TextureFormatType Texture::GetFormatType() const { return m_formatType; }

}  // namespace SD
