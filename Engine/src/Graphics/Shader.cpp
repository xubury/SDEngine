#include "Graphics/Shader.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/OpenGL/GLShader.hpp"
#include "Utils/Log.hpp"

namespace sd {

Ref<Shader> Shader::create() {
    Ref<Shader> shader;
    switch (Renderer::getAPI()) {
        case API::OpenGL:
            shader = createRef<GLShader>();
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    if (shader) shader->init();
    return shader;
}
}  // namespace sd
