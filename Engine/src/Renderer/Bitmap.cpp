#include "Renderer/Bitmap.hpp"

namespace SD {

Bitmap::Bitmap(uint32_t width, uint32_t height, uint8_t channels)
    : m_width(width), m_height(height), m_channels(channels) {
    m_data = static_cast<uint8_t *>(calloc(GetPixelsSize(), 1));
}

Bitmap::Bitmap(uint32_t width, uint32_t height, uint8_t channels, uint8_t *data)
    : m_width(width), m_height(height), m_channels(channels) {
    size_t pixel_size = GetPixelsSize();
    m_data = static_cast<uint8_t *>(malloc(pixel_size));
    memcpy(m_data, data, pixel_size);
}

Bitmap::~Bitmap() { free(m_data); }

const uint8_t *Bitmap::Data() const { return m_data; }

uint8_t *Bitmap::Data() { return m_data; }

uint32_t Bitmap::Width() const { return m_width; }

uint32_t Bitmap::Height() const { return m_height; }

uint8_t Bitmap::Channels() const { return m_channels; }

size_t Bitmap::GetPixelsSize() const { return m_width * m_height * m_channels; }

bool Bitmap::HasAlpha() const { return m_channels == 4; }

uint8_t Bitmap::GetPixel(uint32_t x, uint32_t y, uint8_t channel) const {
    return m_data[(x + y * m_width) * m_channels + channel];
}

void Bitmap::GetPixels(uint32_t x, uint32_t y, uint32_t width, uint32_t height,
                       uint8_t *value) const {
    const size_t pitch = width * m_channels;
    for (; height > 0; --height) {
        std::copy(
            m_data + (x + (y + height - 1) * m_width) * m_channels,
            m_data + (x + (y + height - 1) * m_width) * m_channels + pitch,
            value + (height - 1) * pitch);
    }
}

void Bitmap::SetPixel(uint32_t x, uint32_t y, uint8_t channel, uint8_t value) {
    m_data[(x + y * m_width) * m_channels + channel] = value;
}

void Bitmap::SetPixels(uint32_t x, uint32_t y, uint32_t width, uint32_t height,
                       uint8_t *value) {
    const size_t pitch = width * m_channels;
    for (; height > 0; --height) {
        std::copy(value + (height - 1) * pitch, value + height * pitch,
                  m_data + (x + (y + height - 1) * m_width) * m_channels);
    }
}

void Bitmap::Flip() {
    const size_t pitch = m_width * m_channels;
    uint8_t *tmp = new uint8_t[pitch];
    for (uint32_t y = 0; y < m_height / 2; ++y) {
        std::copy(m_data + y * pitch, m_data + (y + 1) * pitch, tmp);
        std::copy(m_data + (m_height - y - 1) * pitch,
                  m_data + (m_height - y) * pitch, m_data + y * pitch);
        std::copy(tmp, tmp + pitch, m_data + (m_height - y - 1) * pitch);
    }
    delete[] tmp;
}

}  // namespace SD
