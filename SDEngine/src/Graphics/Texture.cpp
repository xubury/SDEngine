#include "Graphics/Texture.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Graphics/OpenGL/GLTexture.hpp"

namespace SD {

Ref<Texture> Texture::Create(int width, int height, int depth,
                             MultiSampleLevel samples, TextureType type,
                             DataFormat format, TextureParameter params,
                             int32_t mipmap_levels)
{
    Ref<Texture> texture;
    switch (Device::GetAPI()) {
        case Device::API::OpenGL:
            texture =
                CreateRef<GLTexture>(width, height, depth, samples, type,
                                     format, std::move(params), mipmap_levels);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return texture;
}

bool Texture::operator==(const Texture &other) const
{
    return Handle() == other.Handle();
}

bool Texture::operator!=(const Texture &other) const
{
    return !(*this == other);
}

size_t Texture::GetDataSize() const
{
    int stride = 0;
    switch (GetFormat()) {
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
    return GetWidth() * GetHeight() * stride;
}

}  // namespace SD
