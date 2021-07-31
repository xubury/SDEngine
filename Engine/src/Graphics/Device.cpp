#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLDevice.hpp"
#include "Core/Log.hpp"

namespace sd {

API Device::s_api;

Scope<Device> Device::s_instance;

void Device::init(API api) {
    s_api = api;
    switch (api) {
        case API::OpenGL:
            s_instance = createScope<GLDevice>();
            break;
        case API::None:
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
}

API Device::getAPI() { return s_api; }

Device &Device::instance() { return *s_instance; }

}  // namespace sd
