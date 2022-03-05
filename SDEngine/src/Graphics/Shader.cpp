#include "Graphics/Shader.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLShader.hpp"

namespace SD {

Ref<Shader> Shader::Create() {
    Ref<Shader> shader;
    switch (Device::GetAPI()) {
        case Device::API::OpenGL:
            shader = CreateRef<GLShader>();
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return shader;
}

}  // namespace SD
