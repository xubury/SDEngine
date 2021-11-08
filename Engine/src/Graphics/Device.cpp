#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLDevice.hpp"
#include "Utility/Log.hpp"

namespace sd {

Ref<Device> Device::create() {
    Ref<Device> device;
    switch (getGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            device = createRef<GLDevice>();
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return device;
}

void Device::submit(const VertexArray &vao, MeshTopology topology, size_t count,
                    size_t offset) {
    vao.bind();
    drawElements(topology, count, offset);
}

}  // namespace sd
