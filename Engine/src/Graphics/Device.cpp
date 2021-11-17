#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLDevice.hpp"

namespace SD {

Ref<Device> Device::Create() {
    Ref<Device> device;
    switch (GetGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            device = CreateRef<GLDevice>();
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return device;
}

}  // namespace SD
