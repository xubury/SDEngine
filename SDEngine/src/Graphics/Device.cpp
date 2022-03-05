#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLDevice.hpp"

namespace SD {

static Device *s_device = nullptr;

void Device::Init() {
    SD_CORE_TRACE("Initializing Graphics Deivce...");
    switch (GetGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            s_device = new GLDevice();
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
}

void Device::Shutdown() {
    delete s_device;
    s_device = nullptr;
}

Device &Device::Get() { return *s_device; }

}  // namespace SD
