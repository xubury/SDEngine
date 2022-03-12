#include "Graphics/Texture.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Graphics/OpenGL/GLTexture.hpp"

namespace SD {

Ref<Texture> Texture::Create(const TextureSpec &spec) {
    Ref<Texture> texture;
    switch (Device::GetAPI()) {
        case Device::API::OpenGL:
            texture = CreateRef<GLTexture>(spec.width, spec.height, spec.depth,
                                           spec.samples, spec.type, spec.format,
                                           spec.format_type, spec.wrap,
                                           spec.filter, spec.min_filter);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return texture;
}

Texture::Texture(int width, int height, int depth, int samples,
                 TextureType type, DataFormat format,
                 DataFormatType format_type, TextureWrap wrap,
                 TextureMagFilter filter, TextureMinFilter min_filter)
    : m_width(width),
      m_height(height),
      m_depth(depth),
      m_samples(samples),
      m_type(type),
      m_format(format),
      m_format_type(format_type),
      m_wrap(wrap),
      m_filter(filter),
      m_min_filter(min_filter) {
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

int Texture::GetSamples() const { return m_samples; }

TextureType Texture::GetType() const { return m_type; }

DataFormat Texture::GetFormat() const { return m_format; }

DataFormatType Texture::GetFormatType() const { return m_format_type; }

}  // namespace SD
