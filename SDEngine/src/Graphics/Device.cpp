#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLDevice.hpp"

namespace SD {

static Device *s_device = nullptr;

static Device::API s_api = Device::API::OpenGL;

Device::API Device::GetAPI() { return s_api; }
void Device::SetAPI(API api) { s_api = api; }

void Device::Init() {
    SD_CORE_TRACE("Initializing Graphics Deivce...");
    switch (s_api) {
        case API::OpenGL:
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
