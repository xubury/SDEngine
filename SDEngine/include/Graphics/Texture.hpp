#ifndef SD_TEXTURE_HPP
#define SD_TEXTURE_HPP

#include "Graphics/Image.hpp"
#include "Utility/Base.hpp"
#include "Graphics/Graphics.hpp"

namespace SD {

struct TextureParameter {
    TextureWrap wrap{TextureWrap::Edge};
    TextureMinFilter min_filter{TextureMinFilter::Nearest};
    TextureMagFilter mag_filter{TextureMagFilter::Nearest};
    MipmapMode mipmap{MipmapMode::None};
};

class SD_GRAPHICS_API Texture {
   public:
    static Ref<Texture> Create(int width, int height, int depth,
                               MultiSampleLevel samples, TextureType type,
                               DataFormat format, TextureParameter params = {},
                               int32_t mipmap_levels = 0);

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

    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual MultiSampleLevel GetSamples() const = 0;

    virtual TextureType GetType() const = 0;
    virtual DataFormat GetFormat() const = 0;

    virtual TextureWrap GetWrap() const = 0;
    virtual TextureMinFilter GetMinFilter() const = 0;
    virtual TextureMagFilter GetMagFilter() const = 0;
    virtual MipmapMode GetMipmapMode() const = 0;
    virtual int GetMipmapLevels() const = 0;

    size_t GetDataSize() const;

   protected:
    Texture() = default;
};

}  // namespace SD

#endif /* SD_TEXTURE_HPP */
