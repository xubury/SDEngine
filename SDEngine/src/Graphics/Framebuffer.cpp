#include "Graphics/Framebuffer.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLFramebuffer.hpp"

namespace SD {

Ref<Framebuffer> Framebuffer::Create() {
    Ref<Framebuffer> frameBuffer;
    switch (Device::GetAPI()) {
        case Device::API::OpenGL:
            frameBuffer = CreateRef<GLFramebuffer>();
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return frameBuffer;
}

void Framebuffer::Attach(const TextureSpec &spec) {
    m_texture_specs.push_back(spec);
}

void Framebuffer::Attach(const RenderbufferSpec &spec) {
    m_renderbuffer_specs.push_back(spec);
}

}  // namespace SD
