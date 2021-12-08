#ifndef SD_TEXTURE_HPP
#define SD_TEXTURE_HPP

#include "Utility/Base.hpp"
#include "Graphics/Graphics.hpp"

namespace SD {

struct SD_GRAPHICS_API TextureSpec {
    uint8_t samples;
    TextureType type;
    TextureFormat format;
    TextureFormatType format_type;
    TextureWrap wrap;
    TextureFilter filter;
    TextureMipmapFilter mipmap_filter;
    TextureSpec(uint8_t samples, TextureType type, TextureFormat format,
                TextureFormatType format_type,
                TextureWrap wrap = TextureWrap::REPEAT,
                TextureFilter filter = TextureFilter::LINEAR,
                TextureMipmapFilter mipmap_filter = TextureMipmapFilter::LINEAR)
        : samples(samples),
          type(type),
          format(format),
          format_type(format_type),
          wrap(wrap),
          filter(filter),
          mipmap_filter(mipmap_filter) {}
};

class SD_GRAPHICS_API Texture {
   public:
    static Ref<Texture> Create(
        int width, int height, int samples, TextureType type,
        TextureFormat format, TextureFormatType format_type,
        TextureWrap wrap = TextureWrap::REPEAT,
        TextureFilter filter = TextureFilter::LINEAR,
        TextureMipmapFilter mipmap_filter = TextureMipmapFilter::LINEAR);

    virtual ~Texture() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual void SetSlot(uint32_t slot) const = 0;

    // Set the base level pixel data (TODO: Does SetPixels for other levels
    // really needed?)
    virtual void SetPixels(int x, int y, int z, size_t width, size_t height,
                           size_t depth, const void *data) = 0;
    virtual void SetBorderColor(const void *color) = 0;
    virtual void SetWrap(TextureWrap wrap) = 0;
    virtual void SetFilter(TextureFilter filter) = 0;
    virtual void SetMipmapFilter(TextureMipmapFilter filter) = 0;

    virtual void ReadPixels(int level, int x, int y, int z, int w, int h, int d,
                            size_t size, void *data) const = 0;

    bool operator==(const Texture &other) const;
    bool operator!=(const Texture &other) const;

    virtual uint32_t GetId() const = 0;

    int GetWidth() const;
    int GetHeight() const;
    int GetSamples() const;

    TextureType GetType() const;
    TextureFormat GetFormat() const;
    TextureFormatType GetFormatType() const;

   protected:
    Texture(int width, int height, int samples, TextureType type,
            TextureFormat format, TextureFormatType format_type,
            TextureWrap wrap, TextureFilter filter,
            TextureMipmapFilter mipmap_filter);

    int m_width;
    int m_height;
    int m_samples;
    int m_mipmap_levels;

    TextureType m_type;
    TextureFormat m_format;
    TextureFormatType m_format_type;
    TextureWrap m_wrap;
    TextureFilter m_filter;
    TextureMipmapFilter m_mipmap_filter;
};

}  // namespace SD

#endif /* SD_TEXTURE_HPP */
