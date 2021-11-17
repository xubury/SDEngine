#include "Graphics/Framebuffer.hpp"
#include "Graphics/OpenGL/GLFramebuffer.hpp"

namespace SD {

Ref<Framebuffer> Framebuffer::Create() {
    Ref<Framebuffer> frameBuffer;
    switch (GetGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            frameBuffer = CreateRef<GLFramebuffer>();
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return frameBuffer;
}

}  // namespace SD
