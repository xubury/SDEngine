#ifndef SD_BITMAP_HPP
#define SD_BITMAP_HPP

#include "Utility/Base.hpp"
#include "Renderer/Export.hpp"

namespace SD {

class SD_RENDERER_API Bitmap {
   public:
    Bitmap(uint32_t width, uint32_t height, uint8_t channels);
    Bitmap(uint32_t width, uint32_t height, uint8_t channels, uint8_t *data);
    ~Bitmap();

    Bitmap(const Bitmap &) = delete;
    Bitmap &operator=(const Bitmap &) = delete;

    const uint8_t *Data() const;
    uint8_t *Data();

    uint32_t Width() const;
    uint32_t Height() const;
    uint8_t Channels() const;

    bool HasAlpha() const;

    size_t GetPixelsSize() const;

    uint8_t GetPixel(uint32_t x, uint32_t y, uint8_t channel) const;
    void GetPixels(uint32_t x, uint32_t y, uint32_t width, uint32_t height,
                   uint8_t *value) const;

    void SetPixel(uint32_t x, uint32_t y, uint8_t channel, uint8_t value);
    void SetPixels(uint32_t x, uint32_t y, uint32_t width, uint32_t height,
                   uint8_t *value);

    void Flip();
   private:
    uint32_t m_width;
    uint32_t m_height;
    uint8_t m_channels;
    uint8_t *m_data;
};

}  // namespace SD

#endif  // SD_BITMAP_HPP
