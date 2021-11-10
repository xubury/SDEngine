#include "Graphics/Shader.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/OpenGL/GLShader.hpp"
#include "Utility/Log.hpp"
#include "Utility/Assert.hpp"

namespace sd {

Ref<Shader> Shader::create(const std::string& filePath) {
    Ref<Shader> shader;
    switch (getGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            shader = createRef<GLShader>(filePath);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return shader;
}

void ShaderLibrary::add(const std::string& name, const Ref<Shader>& shader) {
    SD_CORE_ASSERT(!exists(name), "Shader already exists!");
    m_Shaders[name] = shader;
}

Ref<Shader> ShaderLibrary::load(const std::string& filepath) {
    auto shader = Shader::create(getAbsolutePath(filepath));
    add(filepath, shader);
    return shader;
}

Ref<Shader> ShaderLibrary::load(const std::string& name,
                                const std::string& filepath) {
    auto shader = Shader::create(getAbsolutePath(filepath));
    add(name, shader);
    return shader;
}

Ref<Shader> ShaderLibrary::get(const std::string& name) {
    SD_CORE_ASSERT(exists(name), "Shader not found!");
    return m_Shaders[name];
}

bool ShaderLibrary::exists(const std::string& name) const {
    return m_Shaders.find(name) != m_Shaders.end();
}
}  // namespace sd
