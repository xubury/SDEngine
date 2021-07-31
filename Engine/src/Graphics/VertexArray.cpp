#include "Graphics/VertexArray.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLVertexArray.hpp"
#include "Core/Log.hpp"

namespace sd {

Ref<VertexArray> VertexArray::create() {
    Ref<VertexArray> va;
    switch (Device::getAPI()) {
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
