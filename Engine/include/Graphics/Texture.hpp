#ifndef SD_TEXTURE_HPP
#define SD_TEXTURE_HPP

#include "Core/Export.hpp"
#include "Core/Base.hpp"
#include "Graphics/RendererAPI.hpp"

namespace sd {

class SD_API Texture {
   public:
    static Ref<Texture> create(int width, int height, TextureType type,
                               TextureFormat format,
                               TextureFormatType formatType, TextureWrap wrap,
                               TextureFilter filter,
                               TextureMipmapFilter mipmapFilter,
                               const void *data = nullptr);

    virtual ~Texture() = default;

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual void setPixels(int width, int height, const void *data) = 0;
    virtual void setBorderColor(const void *color) = 0;
    virtual void setWrap(TextureWrap wrap) = 0;
    virtual void setFilter(TextureFilter filter) = 0;
    virtual void setMipmapFilter(TextureMipmapFilter filter) = 0;

    virtual void genareteMipmap() const = 0;

    virtual uint32_t id() const = 0;

    bool operator==(const Texture &other) const;

   protected:
    Texture(int width, int height, TextureType type, TextureFormat format,
            TextureFormatType formatType, TextureWrap wrap,
            TextureFilter filter, TextureMipmapFilter mipmapFilter,
            const void *data);

    virtual void init() = 0;

    int m_width;
    int m_height;

    TextureType m_type;
    TextureFormat m_format;
    TextureFormatType m_formatType;
    TextureWrap m_wrap;
    TextureFilter m_filter;
    TextureMipmapFilter m_mipmapFilter;
    const void *m_data;
};

}  // namespace sd

#endif /* SD_TEXTURE_HPP */
