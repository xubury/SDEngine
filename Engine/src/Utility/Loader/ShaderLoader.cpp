#include "Utility/Loader/ShaderLoader.hpp"
#include "Utility/File.hpp"
#include "Utility/Assert.hpp"

namespace sd {

static ShaderType shaderTypeFromName(const std::string &name) {
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

ShaderLoader::ShaderLoader(AssetManager &manager)
    : AssetLoader<Shader>(manager) {}

Ref<Shader> ShaderLoader::loadAsset(const std::string &filePath) {
    Ref<Shader> shader = Shader::create();
    std::string source;
    SD_CORE_TRACE("Building shader code from {}", filePath);
    readFile(filePath, source);

    size_t i = source.find("#shader");
    bool success = true;
    while (i < source.size()) {
        size_t start = source.find('\n', i) + 1;
        if (i != 0 && start == 0) {
            start = source.find('\0', i) + 1;
        }

        size_t offset = i + 8;
        std::string name = source.substr(offset, start - offset - 1);

        size_t end = source.find("#shader", start);
        i = end;

        ShaderType type = shaderTypeFromName(name);

        std::string code;
        if (type == ShaderType::INVALID) {
            SD_CORE_ERROR("Invalid shader type: {}", name);
            success = false;
            break;
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
            std::string include = code.substr(offset, start - offset - 1);

            code.erase(j, start - j);
            std::string includeCode;
            readFile(m_manager.getAbsolutePath(include).string(), includeCode);
            code.insert(j, includeCode);

            j = code.find("#include", start);
        }

        shader->compileShader(type, code.c_str());
    }
    SD_CORE_ASSERT(success, "Error building shader code");
    shader->linkShaders();
    return shader;
}

}  // namespace sd
