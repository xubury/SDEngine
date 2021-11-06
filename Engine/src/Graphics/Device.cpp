#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLDevice.hpp"
#include "Utility/Log.hpp"

namespace sd {

static Device *s_device;

void Device::init() {
    SD_CORE_TRACE("Initializing Graphics Device...");
    switch (Graphics::getAPI()) {
        case API::OpenGL:
            s_device = new GLDevice();
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
}

Device &Device::instance() { return *s_device; }

void Device::submit(const VertexArray &vao, MeshTopology topology, size_t count,
                    size_t offset) {
    vao.bind();
    drawElements(topology, count, offset);
}

}  // namespace sd
