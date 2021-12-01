#include "Loader/ShaderLoader.hpp"
#include "Graphics/Shader.hpp"
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

Ref<void> ShaderLoader::LoadAsset(const std::string& path) {
    std::string source;
    Ref<Shader> shader = Shader::Create();
    SD_CORE_TRACE("Building shader code from {}", path);
    File::Read(path, source);
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
            throw FileException(path,
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
                File::Read(Manager().GetAbsolutePath(include).string(),
                           includeCode);
            } catch (const FileException& e) {
                throw FileException(
                    path, fmt::format("Invalid include path: {}", include));
            }
            code.insert(j, includeCode);

            j = code.find("#include", start);
        }
        shader->CompileShader(type, code.c_str());
    }
    shader->LinkShaders();
    return shader;
}

}  // namespace SD
