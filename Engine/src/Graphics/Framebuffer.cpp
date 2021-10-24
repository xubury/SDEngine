#include "Graphics/Framebuffer.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/OpenGL/GLFramebuffer.hpp"
#include "Utility/Log.hpp"

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
    return frameBuffer;
}

}  // namespace sd
