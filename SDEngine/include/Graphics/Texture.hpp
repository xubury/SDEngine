#ifndef SD_TEXTURE_HPP
#define SD_TEXTURE_HPP

#include "Utility/Base.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Resource.hpp"

namespace SD {

class SD_GRAPHICS_API Texture : public Resource {
   public:
    static Ref<Texture> Create(
        int width, int height, int depth, MultiSampleLevel samples,
        TextureType type, DataFormat format,
        TextureWrap wrap = TextureWrap::Edge,
        TextureMinFilter min_filter = TextureMinFilter::Nearest,
        MipmapMode mode = MipmapMode::None,
        TextureMagFilter mag_filter = TextureMagFilter::Nearest);

    virtual ~Texture() = default;

    // Set the base level pixel data (TODO: Does SetPixels for other levels
    // really needed?)
    virtual void SetPixels(int x, int y, int z, int width, int height,
                           int depth, const void *data) = 0;
    virtual void SetBorderColor(const void *color) = 0;
    virtual void SetWrap(TextureWrap wrap) = 0;
    virtual void SetMagFilter(TextureMagFilter filter) = 0;
    virtual void SetMinFilter(TextureMinFilter filter) = 0;
    virtual void SetMipmapMode(MipmapMode mode) = 0;

    virtual void ReadPixels(int level, int x, int y, int z, int w, int h, int d,
                            size_t size, void *data) const = 0;

    bool operator==(const Texture &other) const;
    bool operator!=(const Texture &other) const;

    int GetWidth() const;
    int GetHeight() const;
    MultiSampleLevel GetSamples() const;

    TextureType GetType() const;
    DataFormat GetFormat() const;


    TextureWrap GetWrap() const { return m_wrap; }
    TextureMinFilter GetMinFilter() const { return m_min_filter; }
    TextureMagFilter GetMagFilter() const { return m_mag_filter; }
    MipmapMode GetMipmapMode() const { return m_mipmap_mode; }

   protected:
    Texture(int width, int height, int depth, MultiSampleLevel samples,
            TextureType type, DataFormat format, TextureWrap wrap,
            TextureMinFilter min_filter, MipmapMode mode,
            TextureMagFilter mag_filter);

    int m_width;
    int m_height;
    int m_depth;
    MultiSampleLevel m_samples;
    int m_mipmap_levels;

    TextureType m_type;
    DataFormat m_format;
    TextureWrap m_wrap;
    MipmapMode m_mipmap_mode;
    TextureMinFilter m_min_filter;
    TextureMagFilter m_mag_filter;
};

}  // namespace SD

#endif /* SD_TEXTURE_HPP */
