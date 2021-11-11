#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLDevice.hpp"
#include "Utility/Log.hpp"

namespace SD {

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

}  // namespace SD
