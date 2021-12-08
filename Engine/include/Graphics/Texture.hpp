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
    TextureMagFilter filter;
    TextureMinFilter min_filter;
    TextureSpec(uint8_t samples, TextureType type, TextureFormat format,
                TextureFormatType format_type,
                TextureWrap wrap = TextureWrap::REPEAT,
                TextureMagFilter filter = TextureMagFilter::LINEAR,
                TextureMinFilter min_filter = TextureMinFilter::LINEAR_LINEAR)
        : samples(samples),
          type(type),
          format(format),
          format_type(format_type),
          wrap(wrap),
          filter(filter),
          min_filter(min_filter) {}
};

class SD_GRAPHICS_API Texture {
   public:
    static Ref<Texture> Create(int width, int height, const TextureSpec &spec);

    virtual ~Texture() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual void SetSlot(uint32_t slot) const = 0;

    // Set the base level pixel data (TODO: Does SetPixels for other levels
    // really needed?)
    virtual void SetPixels(int x, int y, int z, int width, int height,
                           int depth, const void *data) = 0;
    virtual void SetBorderColor(const void *color) = 0;
    virtual void SetWrap(TextureWrap wrap) = 0;
    virtual void SetMagFilter(TextureMagFilter filter) = 0;
    virtual void SetMinFilter(TextureMinFilter filter) = 0;

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
            TextureWrap wrap, TextureMagFilter filter,
            TextureMinFilter min_filter);

    int m_width;
    int m_height;
    int m_samples;
    int m_mipmap_levels;

    TextureType m_type;
    TextureFormat m_format;
    TextureFormatType m_format_type;
    TextureWrap m_wrap;
    TextureMagFilter m_filter;
    TextureMinFilter m_min_filter;
};

}  // namespace SD

#endif /* SD_TEXTURE_HPP */
