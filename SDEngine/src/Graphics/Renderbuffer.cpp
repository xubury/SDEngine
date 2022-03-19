#include "Graphics/Renderbuffer.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLRenderbuffer.hpp"

namespace SD {
Ref<Renderbuffer> Renderbuffer::Create(int32_t width, int32_t height,
                                       MultiSampleLevel samples,
                                       DataFormat format)
{
    Ref<Renderbuffer> texture;
    switch (Device::GetAPI()) {
        case Device::API::OpenGL:
            texture = CreateRef<GLRenderbuffer>(width, height, samples, format);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return texture;
}

Renderbuffer::Renderbuffer(int width, int height, MultiSampleLevel samples,
                           DataFormat format)
    : m_width(width), m_height(height), m_samples(samples), m_format(format)
{
}

}  // namespace SD
