#include "Graphics/Shader.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/OpenGL/GLShader.hpp"

namespace SD {

Ref<Shader> Shader::Create() {
    Ref<Shader> shader;
    switch (GetGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            shader = CreateRef<GLShader>();
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return shader;
}

}  // namespace SD
