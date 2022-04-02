#include "Graphics/Framebuffer.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLFramebuffer.hpp"

namespace SD {

Ref<Framebuffer> Framebuffer::Create()
{
    Ref<Framebuffer> framebuffer;
    switch (Device::GetAPI()) {
        case Device::API::OpenGL:
            framebuffer = CreateRef<GLFramebuffer>();
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return framebuffer;
}

}  // namespace SD
