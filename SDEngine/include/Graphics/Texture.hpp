#ifndef SD_TEXTURE_HPP
#define SD_TEXTURE_HPP

#include "Utility/Base.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Resource.hpp"

namespace SD {

struct SD_GRAPHICS_API TextureSpec {
    int width;
    int height;
    int depth;
    uint8_t samples;
    TextureType type;
    DataFormat format;
    DataFormatType format_type;
    TextureWrap wrap;
    TextureMagFilter filter;
    TextureMinFilter min_filter;
    TextureSpec(int width, int height, int depth, uint8_t samples,
                TextureType type, DataFormat format, DataFormatType format_type,
                TextureWrap wrap = TextureWrap::REPEAT,
                TextureMagFilter filter = TextureMagFilter::LINEAR,
                TextureMinFilter min_filter = TextureMinFilter::LINEAR_LINEAR)
        : width(width),
          height(height),
          depth(depth),
          samples(samples),
          type(type),
          format(format),
          format_type(format_type),
          wrap(wrap),
          filter(filter),
          min_filter(min_filter) {}
};

class SD_GRAPHICS_API Texture : public Resource {
   public:
    static Ref<Texture> Create(const TextureSpec &spec);

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

    int GetWidth() const;
    int GetHeight() const;
    int GetSamples() const;

    TextureType GetType() const;
    DataFormat GetFormat() const;
    DataFormatType GetFormatType() const;

   protected:
    Texture(int width, int height, int depth, int samples, TextureType type,
            DataFormat format, DataFormatType format_type, TextureWrap wrap,
            TextureMagFilter filter, TextureMinFilter min_filter);

    int m_width;
    int m_height;
    int m_depth;
    int m_samples;
    int m_mipmap_levels;

    TextureType m_type;
    DataFormat m_format;
    DataFormatType m_format_type;
    TextureWrap m_wrap;
    TextureMagFilter m_filter;
    TextureMinFilter m_min_filter;
};

}  // namespace SD

#endif /* SD_TEXTURE_HPP */
