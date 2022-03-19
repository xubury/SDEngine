#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLDevice.hpp"

namespace SD {

static Device::API s_api = Device::API::OpenGL;

Device::API Device::GetAPI() { return s_api; }

void Device::SetAPI(API api) { s_api = api; }

Scope<Device> Device::Create()
{
    SD_CORE_TRACE("Initializing Graphics Deivce...");
    Scope<Device> device;
    switch (s_api) {
        case API::OpenGL:
            device = CreateScope<GLDevice>();
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return device;
}

}  // namespace SD
