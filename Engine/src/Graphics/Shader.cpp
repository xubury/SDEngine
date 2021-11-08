#include "Graphics/Shader.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/OpenGL/GLShader.hpp"
#include "Utility/Log.hpp"

namespace sd {

Ref<Shader> Shader::create() {
    Ref<Shader> shader;
    switch (getGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            shader = createRef<GLShader>();
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return shader;
}
}  // namespace sd
