#ifndef SD_IMAGE_HPP
#define SD_IMAGE_HPP

#include "Graphics/Graphics.hpp"
#include "Utility/Base.hpp"
#include "Graphics/Export.hpp"
#include "Resource/Resource.hpp"

namespace SD {

class SD_GRAPHICS_API ByteImage : public Resource {
   public:
    ByteImage(int32_t width, int32_t height, int8_t channels);
    ByteImage(int32_t width, int32_t height, int8_t channels, uint8_t *data);
    ~ByteImage();

    ByteImage(const ByteImage &) = delete;
    ByteImage &operator=(const ByteImage &) = delete;

    DataFormat GetDataFormat() const;

    const uint8_t *Data() const;
    uint8_t *Data();

    uint32_t Width() const;
    uint32_t Height() const;
    uint8_t Channels() const;

    bool HasAlpha() const;

    size_t GetPixelsSize() const;

    uint8_t GetPixel(int32_t x, int32_t y, int8_t channel) const;
    void GetPixels(int32_t x, int32_t y, int32_t width, int32_t height,
                   uint8_t *value) const;

    void SetPixel(int32_t x, int32_t y, int8_t channel, uint8_t value);
    void SetPixels(int32_t x, int32_t y, int32_t width, int32_t height,
                   uint8_t *value);

   private:
    uint32_t m_width;
    uint32_t m_height;
    uint8_t m_channels;
    uint8_t *m_data;
};

}  // namespace SD

#endif  // SD_BITMAP_HPP
