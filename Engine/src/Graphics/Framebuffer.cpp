#include "Graphics/Framebuffer.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/OpenGL/GLFramebuffer.hpp"
#include "Utils/Log.hpp"

namespace sd {

Ref<Framebuffer> Framebuffer::create() {
    Ref<Framebuffer> frameBuffer;
    switch (Renderer::getAPI()) {
        case API::OpenGL:
            frameBuffer = createRef<GLFramebuffer>();
            break;
        case API::None:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    if (frameBuffer) frameBuffer->init();
    return frameBuffer;
}

}  // namespace sd
