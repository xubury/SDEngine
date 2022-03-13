#ifndef SD_TEXTURE_HPP
#define SD_TEXTURE_HPP

#include "Utility/Base.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Resource.hpp"

namespace SD {

class SD_GRAPHICS_API Texture : public Resource {
   public:
    static Ref<Texture> Create(
        int width, int height, int depth, int8_t samples, TextureType type,
        DataFormat format, TextureWrap wrap = TextureWrap::EDGE,
        TextureMinFilter min_filter = TextureMinFilter::NEAREST,
        TextureMagFilter mag_filter = TextureMagFilter::NEAREST);

    virtual ~Texture() = default;

    // virtual void Bind() const = 0;
    // virtual void Unbind() const = 0;

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

    void SetPath(const std::string &path) { m_path = path; }
    const std::string &GetPath() const { return m_path; }

   protected:
    Texture(int width, int height, int depth, int8_t samples, TextureType type,
            DataFormat format, TextureWrap wrap, TextureMinFilter min_filter,
            TextureMagFilter mag_filter);

    int m_width;
    int m_height;
    int m_depth;
    int m_samples;
    int m_mipmap_levels;

    TextureType m_type;
    DataFormat m_format;
    TextureWrap m_wrap;
    TextureMinFilter m_min_filter;
    TextureMagFilter m_mag_filter;

    std::string m_path;
};

}  // namespace SD

#endif /* SD_TEXTURE_HPP */
