#include "Graphics/Texture.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Graphics/OpenGL/GLTexture.hpp"
#include "Utility/Log.hpp"

namespace sd {

Ref<Texture> Texture::create(int width, int height, int samples,
                             TextureType type, TextureFormat format,
                             TextureFormatType formatType, TextureWrap wrap,
                             TextureFilter filter,
                             TextureMipmapFilter mipmapFilter,
                             const void *data) {
    Ref<Texture> texture;
    switch (getGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            texture = createRef<GLTexture>(width, height, samples, type, format,
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
    return getId() == other.getId();
}

bool Texture::operator!=(const Texture &other) const {
    return !(*this == other);
}

int Texture::getWidth() const { return m_width; }

int Texture::getHeight() const { return m_height; }

int Texture::getSamples() const { return m_samples; }

TextureType Texture::getType() const { return m_type; }

TextureFormat Texture::getFormat() const { return m_format; }

TextureFormatType Texture::getFormatType() const { return m_formatType; }

}  // namespace sd
