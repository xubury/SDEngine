#ifndef SD_TEXTURE_HPP
#define SD_TEXTURE_HPP

#include "Utils/Export.hpp"
#include "Utils/Base.hpp"
#include "Graphics/Graphics.hpp"

namespace sd {

class SD_API Texture {
   public:
    static Ref<Texture> create(int width, int height, int samples,
                               TextureType type, TextureFormat format,
                               TextureFormatType formatType, TextureWrap wrap,
                               TextureFilter filter,
                               TextureMipmapFilter mipmapFilter,
                               void *data = nullptr);

    virtual ~Texture() = default;

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual void setSlot(uint32_t slot) const = 0;

    virtual void setPixels(int width, int height, void *data) = 0;
    virtual void setBorderColor(const void *color) = 0;
    virtual void setWrap(TextureWrap wrap) = 0;
    virtual void setFilter(TextureFilter filter) = 0;
    virtual void setMipmapFilter(TextureMipmapFilter filter) = 0;

    virtual void genareteMipmap() const = 0;

    bool operator==(const Texture &other) const;

    bool operator!=(const Texture &other) const;

    virtual uint32_t getId() const = 0;

    int getWidth() const;

    int getHeight() const;

    int getSamples() const;

    const void *getData() const;

    void *getData();

    TextureType getType() const;

    TextureFormat getFormat() const;

    TextureFormatType getFormatType() const;

   protected:
    Texture(int width, int height, int samples, TextureType type,
            TextureFormat format, TextureFormatType formatType,
            TextureWrap wrap, TextureFilter filter,
            TextureMipmapFilter mipmapFilter, void *data);

    virtual void init() = 0;

    int m_width;
    int m_height;
    int m_samples;

    TextureType m_type;
    TextureFormat m_format;
    TextureFormatType m_formatType;
    TextureWrap m_wrap;
    TextureFilter m_filter;
    TextureMipmapFilter m_mipmapFilter;
    void *m_data;
};

}  // namespace sd

#endif /* SD_TEXTURE_HPP */
