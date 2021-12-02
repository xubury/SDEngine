#include "Graphics/Shader.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/OpenGL/GLShader.hpp"
#include "Utility/File.hpp"

namespace SD {

static inline ShaderType ShaderTypeFromName(const std::string& name) {
    if (name == "vertex") {
        return ShaderType::VERTEX;
    } else if (name == "fragment") {
        return ShaderType::FRAGMENT;
    } else if (name == "geometry") {
        return ShaderType::GEOMETRY;
    } else if (name == "compute") {
        return ShaderType::COMPUTE;
    }

    return ShaderType::INVALID;
}

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

void ShaderLibrary::BuildShader(Shader& shader, const std::filesystem::path& path) {
    std::string full_path = GetAbsolutePath(path).string();
    SD_CORE_TRACE("Building shader code from {}", full_path);
    std::string source;
    File::Read(full_path, source);
    size_t i = source.find("#shader");
    while (i < source.size()) {
        size_t start = source.find('\n', i) + 1;
        if (i != 0 && start == 0) {
            start = source.find('\0', i) + 1;
        }

        size_t offset = i + 8;
        std::string name = source.substr(offset, start - offset - 1);

        size_t end = source.find("#shader", start);
        i = end;

        ShaderType type = ShaderTypeFromName(name);

        std::string code;
        if (type == ShaderType::INVALID) {
            throw FileException(full_path,
                                fmt::format("Invalid shader type: {}", name));
        } else {
            code = source.substr(start, end - start);
        }
        // insert include code
        size_t j = code.find("#include");
        while (j < code.size()) {
            size_t start = code.find('\n', j) + 1;
            if (j != 0 && start == 0) {
                start = code.find('\0', j) + 1;
            }

            size_t offset = j + 9;
            std::filesystem::path include =
                code.substr(offset, start - offset - 1);

            code.erase(j, start - j);
            std::string includeCode;
            try {
                File::Read(GetAbsolutePath(include).string(), includeCode);
            } catch (const FileException& e) {
                throw FileException(
                    full_path,
                    fmt::format("Invalid include path: {}", include));
            }
            code.insert(j, includeCode);

            j = code.find("#include", start);
        }
        shader.CompileShader(type, code.c_str());
    }
    shader.LinkShaders();
}

void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader) {
    SD_CORE_ASSERT(!Exists(name), "Shader already exists!");
    m_shaders[name] = shader;
}

Ref<Shader> ShaderLibrary::Load(const std::string& filepath) {
    auto shader = Shader::Create();
    BuildShader(*shader, filepath);
    Add(filepath, shader);
    return shader;
}

Ref<Shader> ShaderLibrary::Load(const std::string& name,
                                const std::string& filepath) {
    auto shader = Shader::Create();
    BuildShader(*shader, filepath);
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
