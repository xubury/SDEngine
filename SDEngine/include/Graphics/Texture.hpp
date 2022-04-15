#ifndef SD_TEXTURE_HPP
#define SD_TEXTURE_HPP

#include "Graphics/Image.hpp"
#include "Utility/Base.hpp"
#include "Graphics/Graphics.hpp"

namespace SD {

class SD_GRAPHICS_API Texture  {
   public:
    static Ref<Texture> CreateCube(const std::array<Ref<ByteImage>, 6> &images);
    static Ref<Texture> CreateIcon(const ByteImage &image);
    static Ref<Texture> Create(
        int width, int height, int depth, MultiSampleLevel samples,
        TextureType type, DataFormat format,
        TextureWrap wrap = TextureWrap::Edge,
        TextureMinFilter min_filter = TextureMinFilter::Nearest,
        TextureMagFilter mag_filter = TextureMagFilter::Nearest,
        MipmapMode mode = MipmapMode::None, int32_t mipmap_levels = 0);

    virtual ~Texture() = default;
    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;

    virtual uint32_t Handle() const = 0;

    // Set the base level pixel data (TODO: Does SetPixels for other levels
    // really needed?)
    virtual void SetPixels(int x, int y, int z, int width, int height,
                           int depth, const void *data) = 0;
    virtual void SetBorderColor(const void *color) = 0;
    virtual void SetWrap(TextureWrap wrap) = 0;
    virtual void SetMagFilter(TextureMagFilter filter) = 0;
    virtual void SetMinFilter(TextureMinFilter filter) = 0;
    virtual void SetBaseLevel(int level) = 0;
    virtual void SetMaxLevel(int level) = 0;

    virtual void ReadPixels(int level, int x, int y, int z, int w, int h, int d,
                            size_t size, void *data) const = 0;

    virtual void GenerateMipmap() = 0;

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
    int GetMipmapLevels() const { return m_mipmap_levels; }

    size_t GetDataSize() const;
   protected:
    Texture(int width, int height, int depth, MultiSampleLevel samples,
            TextureType type, DataFormat format, TextureWrap wrap,
            TextureMinFilter min_filter, TextureMagFilter mag_filter,
            MipmapMode mode, int32_t mipmap_levels);

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
