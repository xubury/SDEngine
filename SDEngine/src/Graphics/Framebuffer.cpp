#include "Graphics/Framebuffer.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLFramebuffer.hpp"

namespace SD {

Ref<Framebuffer> Framebuffer::Create(const FramebufferCreateInfo &info)
{
    Ref<Framebuffer> framebuffer;
    switch (Device::GetAPI()) {
        case Device::API::OpenGL:
            framebuffer = CreateRef<GLFramebuffer>(info);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return framebuffer;
}

Framebuffer::Framebuffer(const FramebufferCreateInfo &info) : m_info(info) {}

void Framebuffer::Resize(int32_t width, int32_t height, int32_t depth)
{
    if (m_info.width != width || m_info.height != height ||
        m_info.depth != depth) {
        m_info.width = width;
        m_info.height = height;
        m_info.depth = depth;
        DestoryAttachments();
        SetupAttachments();
    }
}

}  // namespace SD
