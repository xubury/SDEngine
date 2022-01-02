#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLDevice.hpp"

namespace SD {

Ref<Device> Device::Create(Context *context) {
    Ref<Device> device;
    switch (GetGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            device = CreateRef<GLDevice>(context);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return device;
}

Device::Device(Context *context) : m_context(context) {}

void Device::SetTarget(RenderTarget &target) {
    SetFramebuffer(target.GetFramebuffer());
}

glm::ivec2 Device::GetSize() const { return m_context->GetSize(); }

int Device::GetWidth() const { return m_context->GetWidth(); }

int Device::GetHeight() const { return m_context->GetHeight(); }

uint8_t Device::GetMSAA() const { return m_context->GetMSAA(); }

}  // namespace SD
