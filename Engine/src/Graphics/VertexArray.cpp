#include "Graphics/VertexArray.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/OpenGL/GLVertexArray.hpp"
#include "Utility/Log.hpp"

namespace sd {

Ref<VertexArray> VertexArray::create() {
    Ref<VertexArray> va;
    switch (getGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            va = createRef<GLVertexArray>();
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return va;
}

}  // namespace sd
