#include "Graphics/Framebuffer.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLFramebuffer.hpp"

namespace SD {

Ref<Framebuffer> Framebuffer::Create(int32_t width, int32_t height,
                                     int32_t depth, int8_t samples) {
    Ref<Framebuffer> framebuffer;
    switch (Device::GetAPI()) {
        case Device::API::OpenGL:
            framebuffer =
                CreateRef<GLFramebuffer>(width, height, depth, samples);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return framebuffer;
}

Framebuffer::Framebuffer(int32_t width, int32_t height, int32_t depth,
                         int8_t samples)
    : m_width(width), m_height(height), m_depth(depth), m_samples(samples) {}

void Framebuffer::Attach(const AttachmentDescription &desc) {
    m_attachment_descs.emplace_back(desc);
}

void Framebuffer::Resize(int32_t width, int32_t height, int32_t depth) {
    if (m_width != width || m_height != height || m_depth != depth) {
        m_width = width;
        m_height = height;
        m_depth = depth;
        Clear();
        Setup();
    }
}

}  // namespace SD
