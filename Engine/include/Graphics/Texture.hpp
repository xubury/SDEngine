#ifndef SD_TEXTURE_HPP
#define SD_TEXTURE_HPP

#include "Utility/Base.hpp"
#include "Graphics/Graphics.hpp"

namespace SD {

class SD_API Texture {
   public:
    static Ref<Texture> Create(int width, int height, int samples,
                               TextureType type, TextureFormat format,
                               TextureFormatType formatType, TextureWrap wrap,
                               TextureFilter filter,
                               TextureMipmapFilter mipmapFilter,
                               const void *data = nullptr);

    virtual ~Texture() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual void SetSlot(uint32_t slot) const = 0;

    virtual void SetPixels(int width, int height, const void *data) = 0;
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
            TextureFormat format, TextureFormatType formatType,
            TextureWrap wrap, TextureFilter filter,
            TextureMipmapFilter mipmapFilter);

    int m_width;
    int m_height;
    int m_samples;

    TextureType m_type;
    TextureFormat m_format;
    TextureFormatType m_formatType;
    TextureWrap m_wrap;
    TextureFilter m_filter;
    TextureMipmapFilter m_mipmapFilter;
};

}  // namespace SD

#endif /* SD_TEXTURE_HPP */
