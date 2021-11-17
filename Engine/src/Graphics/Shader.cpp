#include "Graphics/Shader.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/OpenGL/GLShader.hpp"

namespace SD {

Ref<Shader> Shader::Create(const std::string& filePath) {
    Ref<Shader> shader;
    switch (GetGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            shader = CreateRef<GLShader>(filePath);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return shader;
}

void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader) {
    SD_CORE_ASSERT(!Exists(name), "Shader already exists!");
    m_shaders[name] = shader;
}

Ref<Shader> ShaderLibrary::Load(const std::string& filepath) {
    auto shader = Shader::Create(GetAbsolutePath(filepath));
    Add(filepath, shader);
    return shader;
}

Ref<Shader> ShaderLibrary::Load(const std::string& name,
                                const std::string& filepath) {
    auto shader = Shader::Create(GetAbsolutePath(filepath));
    Add(name, shader);
    return shader;
}

Ref<Shader> ShaderLibrary::Get(const std::string& name) {
    SD_CORE_ASSERT(Exists(name), "Shader not found!");
    return m_shaders[name];
}

bool ShaderLibrary::Exists(const std::string& name) const {
    return m_shaders.find(name) != m_shaders.end();
}
}  // namespace SD
