#include "Loader/ShaderLoader.hpp"
#include "Graphics/Shader.hpp"
#include "Utility/File.hpp"

namespace SD {

static void ParseShaderCode(const std::string& path, std::string& code) {
    std::filesystem::path directory = std::filesystem::path(path).parent_path();
    const char identifier[] = "#include";
    SD_CORE_TRACE("Building shader code from {}", path);
    File::Read(path, code);
    // insert include code
    size_t i = code.find(identifier);
    while (i < code.size()) {
        size_t start = code.find('\n', i) + 1;
        if (i != 0 && start == 0) {
            start = code.find('\0', i) + 1;
        }

        size_t offset = i + 9;
        std::filesystem::path include = code.substr(offset, start - offset - 1);

        code.erase(i, start - i);
        std::string include_code;
        const std::string include_path = (directory / include).string();
        try {
            File::Read(include_path, include_code);
        } catch (const FileException& e) {
            throw FileException(
                path, fmt::format("Invalid include path: {}", include_path));
        }
        code.insert(i, include_code);

        i = code.find(identifier, start);
    }
}

Ref<Shader> ShaderLoader::LoadShader(const std::string& vert_path,
                                     const std::string& frag_path) {
    Ref<Shader> shader = Shader::Create();
    std::string vert_code;
    std::string frag_code;
    if (!vert_path.empty()) {
        ParseShaderCode(vert_path, vert_code);
        shader->CompileShader(ShaderType::Vertex, vert_code);
    }
    if (!frag_path.empty()) {
        ParseShaderCode(frag_path, frag_code);
        shader->CompileShader(ShaderType::Fragment, frag_code);
    }
    shader->LinkShaders();
    return shader;
}
Ref<Shader> ShaderLoader::LoadShader(const std::string& vert_path,
                                     const std::string& frag_path,
                                     const std::string& geo_path) {
    Ref<Shader> shader = Shader::Create();
    std::string vert_code;
    std::string frag_code;
    std::string geo_code;
    if (!vert_path.empty()) {
        ParseShaderCode(vert_path, vert_code);
        shader->CompileShader(ShaderType::Vertex, vert_code);
    }
    if (!frag_path.empty()) {
        ParseShaderCode(frag_path, frag_code);
        shader->CompileShader(ShaderType::Fragment, frag_code);
    }
    if (!geo_path.empty()) {
        ParseShaderCode(geo_path, geo_code);
        shader->CompileShader(ShaderType::Geometry, geo_code);
    }
    shader->LinkShaders();
    return shader;
}

}  // namespace SD
