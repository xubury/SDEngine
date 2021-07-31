#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLDevice.hpp"
#include "Core/Log.hpp"

namespace sd {

Scope<Device> Device::s_instance;

Device &Device::instance() { return *s_instance; }

}  // namespace sd
