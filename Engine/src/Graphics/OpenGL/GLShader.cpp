#include "Graphics/OpenGL/GLShader.hpp"
#include "Graphics/OpenGL/GLBuffer.hpp"
#include "Graphics/OpenGL/GLTexture.hpp"
#include "Utility/File.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace SD {

static ShaderType ShaderTypeFromName(const std::string& name) {
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

GLShader::GLShader(const std::string& filePath)
    : m_id(0), m_vertexId(0), m_fragmentId(0), m_geometryId(0), m_computeId(0) {
    m_id = glCreateProgram();
    std::string source;
    SD_CORE_TRACE("Building shader code from {}", filePath);
    try {
        File::Read(filePath, source);
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
                SD_CORE_ERROR("Invalid shader type: {}", name);
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
                std::filesystem::path include =
                    code.substr(offset, start - offset - 1);

                code.erase(j, start - j);
                std::string includeCode;
                File::Read(("assets" / include).string(), includeCode);
                code.insert(j, includeCode);

                j = code.find("#include", start);
            }
            CompileShader(type, code.c_str());
        }
        LinkShaders();
    } catch (const FileException& e) {
        SD_CORE_ASSERT(
            false, fmt::format("Error building shader code from {}, error: {}",
                               e.GetFilePath(), e.what()));
    }
}

GLShader::~GLShader() {
    glDeleteProgram(m_id);
    DestroyShaders();
}

void GLShader::CompileShader(ShaderType type, const char* code) {
    switch (type) {
        case ShaderType::VERTEX:
            if (m_vertexId != 0) glDeleteShader(m_vertexId);
            m_vertexId = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(m_vertexId, 1, &code, nullptr);
            glCompileShader(m_vertexId);
            CheckCompileErrors(m_vertexId, "Vertex");
            glAttachShader(m_id, m_vertexId);
            break;
        case ShaderType::FRAGMENT:
            if (m_fragmentId != 0) glDeleteShader(m_fragmentId);
            m_fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(m_fragmentId, 1, &code, nullptr);
            glCompileShader(m_fragmentId);
            CheckCompileErrors(m_fragmentId, "Fragment");
            glAttachShader(m_id, m_fragmentId);
            break;
        case ShaderType::GEOMETRY:
            if (m_geometryId != 0) glDeleteShader(m_geometryId);
            m_geometryId = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(m_geometryId, 1, &code, nullptr);
            glCompileShader(m_geometryId);
            CheckCompileErrors(m_geometryId, "Geometry");
            glAttachShader(m_id, m_geometryId);
            break;
        case ShaderType::COMPUTE:
            if (m_computeId != 0) glDeleteShader(m_computeId);
            m_computeId = glCreateShader(GL_COMPUTE_SHADER);
            glShaderSource(m_computeId, 1, &code, nullptr);
            glCompileShader(m_computeId);
            CheckCompileErrors(m_computeId, "Compute");
            glAttachShader(m_id, m_computeId);
            break;
        case ShaderType::INVALID:
            SD_CORE_ERROR("Trying to add a invalid shader");
            break;
    }
}

void GLShader::DestroyShaders() {
    if (m_vertexId != 0) glDeleteShader(m_vertexId);
    if (m_fragmentId != 0) glDeleteShader(m_fragmentId);
    if (m_geometryId != 0) glDeleteShader(m_geometryId);
    if (m_computeId != 0) glDeleteShader(m_computeId);
    m_vertexId = 0;
    m_fragmentId = 0;
    m_geometryId = 0;
    m_computeId = 0;
}

void GLShader::LinkShaders() {
    glLinkProgram(m_id);
    CheckCompileErrors(m_id, "Program");
    DestroyShaders();
}

void GLShader::Bind() { glUseProgram(m_id); }

void GLShader::CheckCompileErrors(uint32_t shader, const std::string& type) {
    int success;
    int logSize = 0;
    std::string infoLog;
    if (type != "Program") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
        infoLog.resize(logSize);
        if (!success) {
            glGetShaderInfoLog(shader, logSize, nullptr, infoLog.data());
            infoLog = fmt::format("\n{} Shader compilation error\n:{}", type,
                                  infoLog);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &logSize);
        infoLog.resize(logSize);
        if (!success) {
            glGetProgramInfoLog(shader, logSize, nullptr, infoLog.data());
            infoLog =
                fmt::format("\nShader Program linking error\n:{}", infoLog);
        }
    }
    SD_CORE_ASSERT(success, infoLog);
}

void GLShader::SetBool(const std::string& name, bool value) {
    glProgramUniform1i(m_id, glGetUniformLocation(m_id, name.c_str()), value);
}

void GLShader::SetInt(const std::string& name, int value) {
    glProgramUniform1i(m_id, glGetUniformLocation(m_id, name.c_str()), value);
}

void GLShader::SetUint(const std::string& name, uint32_t value) {
    glProgramUniform1ui(m_id, glGetUniformLocation(m_id, name.c_str()), value);
}

void GLShader::SetFloat(const std::string& name, float value) {
    glProgramUniform1f(m_id, glGetUniformLocation(m_id, name.c_str()), value);
}

void GLShader::SetVec2(const std::string& name, const glm::vec2& value) {
    glProgramUniform2fv(m_id, glGetUniformLocation(m_id, name.c_str()), 1,
                        &value[0]);
}

void GLShader::SetVec3(const std::string& name, const glm::vec3& value) {
    glProgramUniform3fv(m_id, glGetUniformLocation(m_id, name.c_str()), 1,
                        &value[0]);
}

void GLShader::SetVec4(const std::string& name, const glm::vec4& value) {
    glProgramUniform4fv(m_id, glGetUniformLocation(m_id, name.c_str()), 1,
                        &value[0]);
}

void GLShader::SetMat4(const std::string& name, const glm::mat4& value) {
    glProgramUniformMatrix4fv(m_id, glGetUniformLocation(m_id, name.c_str()), 1,
                              GL_FALSE, glm::value_ptr(value));
}

void GLShader::SetTexture(const std::string& name, const Texture* texture) {
    uint8_t id = glGetUniformLocation(m_id, name.c_str());
    id = id % 32;
    SetInt(name, id);
    SetTexture(id, texture);
}

void GLShader::SetTexture(uint32_t id, const Texture* texture) {
    if (texture) {
        texture->SetSlot(id);
    } else {
        glBindTextureUnit(id, 0);
    }
}

void GLShader::SetUniformBuffer(const std::string& name,
                                const UniformBuffer& buffer) {
    buffer.Bind();
    uint32_t index = glGetUniformBlockIndex(m_id, name.c_str());
    if (index != GL_INVALID_INDEX)
        glUniformBlockBinding(m_id, index, buffer.GetBindingPoint());
}

}  // namespace SD
