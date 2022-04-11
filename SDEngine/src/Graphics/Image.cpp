#include "Graphics/Image.hpp"

namespace SD {

ByteImage::ByteImage(int32_t width, int32_t height, int8_t channels)
    : m_width(width), m_height(height), m_channels(channels)
{
    m_data = static_cast<uint8_t *>(calloc(GetPixelsSize(), 1));
}

ByteImage::ByteImage(int32_t width, int32_t height, int8_t channels,
                     uint8_t *data)
    : m_width(width), m_height(height), m_channels(channels)
{
    size_t pixel_size = GetPixelsSize();
    m_data = static_cast<uint8_t *>(malloc(pixel_size));
    memcpy(m_data, data, pixel_size);
}

ByteImage::~ByteImage() { free(m_data); }

DataFormat ByteImage::GetDataFormat() const
{
    DataFormat format = DataFormat::RGB8;
    switch (m_channels) {
        case 1:
            format = DataFormat::R8;
            break;
        case 2:
            format = DataFormat::RG8;
            break;
        case 3:
            format = DataFormat::RGB8;
            break;
        case 4:
            format = DataFormat::RGBA8;
            break;
    }
    return format;
}

const uint8_t *ByteImage::Data() const { return m_data; }

uint8_t *ByteImage::Data() { return m_data; }

uint32_t ByteImage::Width() const { return m_width; }

uint32_t ByteImage::Height() const { return m_height; }

uint8_t ByteImage::Channels() const { return m_channels; }

size_t ByteImage::GetPixelsSize() const
{
    return m_width * m_height * m_channels;
}

bool ByteImage::HasAlpha() const { return m_channels == 4; }

uint8_t ByteImage::GetPixel(int32_t x, int32_t y, int8_t channel) const
{
    return m_data[(x + y * m_width) * m_channels + channel];
}

void ByteImage::GetPixels(int32_t x, int32_t y, int32_t width, int32_t height,
                          uint8_t *value) const
{
    const size_t pitch = width * m_channels;
    for (; height > 0; --height) {
        std::copy(
            m_data + (x + (y + height - 1) * m_width) * m_channels,
            m_data + (x + (y + height - 1) * m_width) * m_channels + pitch,
            value + (height - 1) * pitch);
    }
}

void ByteImage::SetPixel(int32_t x, int32_t y, int8_t channel, uint8_t value)
{
    m_data[(x + y * m_width) * m_channels + channel] = value;
}

void ByteImage::SetPixels(int32_t x, int32_t y, int32_t width, int32_t height,
                          uint8_t *value)
{
    const size_t pitch = width * m_channels;
    for (; height > 0; --height) {
        std::copy(value + (height - 1) * pitch, value + height * pitch,
                  m_data + (x + (y + height - 1) * m_width) * m_channels);
    }
}

}  // namespace SD
