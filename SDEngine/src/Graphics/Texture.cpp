#include "Graphics/Texture.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Graphics/OpenGL/GLTexture.hpp"

namespace SD {

Ref<Texture> Texture::CreateCube(const std::array<Ref<ByteImage>, 6> &images)
{
    Ref<Texture> texture;
    for (size_t face = 0; face < images.size(); ++face) {
        auto image = images[face];
        int32_t width = image->Width();
        int32_t height = image->Height();
        if (face == 0) {
            texture =
                Texture::Create(width, height, 0, MultiSampleLevel::None,
                                TextureType::Cube, image->GetDataFormat(),
                                TextureWrap::Edge, TextureMinFilter::Linear,
                                TextureMagFilter::Linear, MipmapMode::None);
        }
        texture->SetPixels(0, 0, face, width, height, 1, image->Data());
    }
    return texture;
}

Ref<Texture> Texture::CreateIcon(const ByteImage &image)
{
    auto texture = Texture::Create(
        image.Width(), image.Height(), 0, MultiSampleLevel::None,
        TextureType::Normal, image.GetDataFormat(), TextureWrap::Edge,
        TextureMinFilter::Linear, TextureMagFilter::Linear, MipmapMode::None);
    texture->SetPixels(0, 0, 0, image.Width(), image.Height(), 1, image.Data());
    return texture;
}

Ref<Texture> Texture::Create(int width, int height, int depth,
                             MultiSampleLevel samples, TextureType type,
                             DataFormat format, TextureWrap wrap,
                             TextureMinFilter min_filter,
                             TextureMagFilter mag_filter, MipmapMode mode,
                             int32_t mipmap_levels)
{
    Ref<Texture> texture;
    switch (Device::GetAPI()) {
        case Device::API::OpenGL:
            texture = CreateRef<GLTexture>(width, height, depth, samples, type,
                                           format, wrap, min_filter, mag_filter,
                                           mode, mipmap_levels);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return texture;
}

Texture::Texture(int width, int height, int depth, MultiSampleLevel samples,
                 TextureType type, DataFormat format, TextureWrap wrap,
                 TextureMinFilter min_filter, TextureMagFilter mag_filter,
                 MipmapMode mode, int32_t mipmap_levels)
    : m_width(width),
      m_height(height),
      m_depth(depth),
      m_samples(samples),
      m_type(type),
      m_format(format),
      m_wrap(wrap),
      m_mipmap_mode(mode),
      m_min_filter(min_filter),
      m_mag_filter(mag_filter)
{
    if (m_mipmap_mode != MipmapMode::None) {
        int max_level = static_cast<int>(
            std::floor(std::log2(std::max(m_width, m_height))));
        if (mipmap_levels == 0) {
            m_mipmap_levels = std::max(max_level, 1);
        }
        else {
            m_mipmap_levels = std::min(mipmap_levels, max_level);
        }
    }
    else {
        m_mipmap_levels = 1;
    }
}

bool Texture::operator==(const Texture &other) const
{
    return Handle() == other.Handle();
}

bool Texture::operator!=(const Texture &other) const
{
    return !(*this == other);
}

int Texture::GetWidth() const { return m_width; }

int Texture::GetHeight() const { return m_height; }

MultiSampleLevel Texture::GetSamples() const { return m_samples; }

TextureType Texture::GetType() const { return m_type; }

DataFormat Texture::GetFormat() const { return m_format; }

size_t Texture::GetDataSize() const
{
    int stride = 0;
    switch (m_format) {
        case DataFormat::Alpha8:
        case DataFormat::R8:
        case DataFormat::Stencil8:
            stride = 1;
            break;
        case DataFormat::Alpha16F:
        case DataFormat::R16F:
        case DataFormat::RG8:
            stride = 2;
            break;
        case DataFormat::RGB8:
        case DataFormat::Depth24:
            stride = 3;
            break;
        case DataFormat::Alpha32F:
        case DataFormat::R32F:
        case DataFormat::Alpha32I:
        case DataFormat::Alpha32UI:
        case DataFormat::R32I:
        case DataFormat::R32UI:
        case DataFormat::RG16F:
        case DataFormat::RGBA8:
        case DataFormat::Depth24Stencil8:
            stride = 4;
            break;
        case DataFormat::RGB16F:
            stride = 6;
            break;
        case DataFormat::RG32F:
        case DataFormat::RG32I:
        case DataFormat::RG32UI:
        case DataFormat::RGBA16F:
            stride = 8;
            break;
        case DataFormat::RGB32F:
        case DataFormat::RGB32UI:
        case DataFormat::RGB32I:
            stride = 12;
            break;
        case DataFormat::RGBA32F:
        case DataFormat::RGBA32I:
        case DataFormat::RGBA32UI:
            stride = 16;
            break;
    }
    return m_width * m_height * stride;
}
}  // namespace SD
