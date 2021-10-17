#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLDevice.hpp"
#include "Utility/Log.hpp"

namespace sd {

static Device *s_device;

void Device::create() {
    SD_CORE_TRACE("Initializing Graphics Device...");
    switch (Graphics::getAPI()) {
        case API::OpenGL:
            s_device = new GLDevice();
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    s_device->init();
}

Device &Device::instance() { return *s_device; }

}  // namespace sd
