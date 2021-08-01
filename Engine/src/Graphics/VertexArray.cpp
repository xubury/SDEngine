#include "Graphics/VertexArray.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/OpenGL/GLVertexArray.hpp"
#include "Utils/Log.hpp"

namespace sd {

Ref<VertexArray> VertexArray::create() {
    Ref<VertexArray> va;
    switch (Renderer::getAPI()) {
        case API::OpenGL:
            va = createRef<GLVertexArray>();
            break;
        case API::None:
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return va;
}

}  // namespace sd
