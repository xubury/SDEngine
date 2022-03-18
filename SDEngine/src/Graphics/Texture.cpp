#include "Graphics/Texture.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Graphics/OpenGL/GLTexture.hpp"

namespace SD {

Ref<Texture> Texture::Create(int width, int height, int depth,
                             MultiSampleLevel samples, TextureType type,
                             DataFormat format, TextureWrap wrap,
                             TextureMinFilter min_filter, MipmapMode mode,
                             TextureMagFilter mag_filter) {
    Ref<Texture> texture;
    switch (Device::GetAPI()) {
        case Device::API::OpenGL:
            texture = CreateRef<GLTexture>(width, height, depth, samples, type,
                                           format, wrap, min_filter, mode,
                                           mag_filter);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return texture;
}

Texture::Texture(int width, int height, int depth, MultiSampleLevel samples,
                 TextureType type, DataFormat format, TextureWrap wrap,
                 TextureMinFilter min_filter, MipmapMode mode,
                 TextureMagFilter mag_filter)
    : m_width(width),
      m_height(height),
      m_depth(depth),
      m_samples(samples),
      m_type(type),
      m_format(format),
      m_wrap(wrap),
      m_mipmap_mode(mode),
      m_min_filter(min_filter),
      m_mag_filter(mag_filter) {
    m_mipmap_levels = std::max(
        static_cast<int>(std::floor(std::log2(std::max(m_width, m_height)))),
        1);
}

bool Texture::operator==(const Texture &other) const {
    return GetId() == other.GetId();
}

bool Texture::operator!=(const Texture &other) const {
    return !(*this == other);
}

int Texture::GetWidth() const { return m_width; }

int Texture::GetHeight() const { return m_height; }

MultiSampleLevel Texture::GetSamples() const { return m_samples; }

TextureType Texture::GetType() const { return m_type; }

DataFormat Texture::GetFormat() const { return m_format; }

}  // namespace SD
