#include "Graphics/VertexArray.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/OpenGL/GLVertexArray.hpp"

namespace SD {

Ref<VertexArray> VertexArray::Create() {
    Ref<VertexArray> va;
    switch (GetGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            va = CreateRef<GLVertexArray>();
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return va;
}

}  // namespace SD
