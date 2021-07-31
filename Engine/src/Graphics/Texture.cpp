#include "Graphics/Texture.hpp"
#include "Graphics/RendererAPI.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Graphics/OpenGL/GLTexture.hpp"

namespace sd {
Ref<Texture> Texture::create(int width, int height, TextureType type,
                             TextureFormat format, TextureFormatType formatType,
                             TextureWrap wrap, TextureFilter filter,
                             TextureMipmapFilter mipmapFilter,
                             const void *data) {
    Ref<Texture> texture;
    switch (RendererAPI::getAPI()) {
        case API::OpenGL:
            texture =
                createRef<GLTexture>(width, height, type, format, formatType,
                                     wrap, filter, mipmapFilter, data);
        case API::None:
        default:
            break;
    }

    if (texture) {
        texture->init();
    }

    return texture;
}

Texture::Texture(int width, int height, TextureType type, TextureFormat format,
                 TextureFormatType formatType, TextureWrap wrap,
                 TextureFilter filter, TextureMipmapFilter mipmapFilter,
                 const void *data)
    : m_width(width),
      m_height(height),
      m_type(type),
      m_format(format),
      m_formatType(formatType),
      m_wrap(wrap),
      m_filter(filter),
      m_mipmapFilter(mipmapFilter),
      m_data(data) {}

}  // namespace sd
