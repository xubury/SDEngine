#include "Graphics/Framebuffer.hpp"
#include "Graphics/OpenGL/GLFramebuffer.hpp"

namespace SD {

Ref<Framebuffer> Framebuffer::create() {
    Ref<Framebuffer> frameBuffer;
    switch (getGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            frameBuffer = createRef<GLFramebuffer>();
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return frameBuffer;
}

}  // namespace SD
