#include "Graphics/Framebuffer.hpp"
#include "Renderer/Renderer.hpp"
#include "Graphics/OpenGL/GLFramebuffer.hpp"
#include "Common/Log.hpp"

namespace sd {

Ref<Framebuffer> Framebuffer::create() {
    Ref<Framebuffer> frameBuffer;
    switch (Graphics::getAPI()) {
        case API::OpenGL:
            frameBuffer = createRef<GLFramebuffer>();
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    if (frameBuffer) frameBuffer->init();
    return frameBuffer;
}

}  // namespace sd
