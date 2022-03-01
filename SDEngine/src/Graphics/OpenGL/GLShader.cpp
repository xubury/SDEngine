#include "Graphics/OpenGL/GLShader.hpp"
#include "Graphics/OpenGL/GLBuffer.hpp"
#include "Graphics/OpenGL/GLTexture.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace SD {

const uint8_t MAX_TEXTURES = 32;

GLShader::GLShader()
    : m_vertexId(0), m_fragmentId(0), m_geometryId(0), m_computeId(0) {
    m_id = glCreateProgram();
}

GLShader::~GLShader() {
    glDeleteProgram(m_id);
    DestroyShaders();
}

void GLShader::CompileShader(ShaderType type, const std::string& code) {
    const char* c_code = code.c_str();
    switch (type) {
        case ShaderType::VERTEX:
            if (m_vertexId != 0) glDeleteShader(m_vertexId);
            m_vertexId = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(m_vertexId, 1, &c_code, nullptr);
            glCompileShader(m_vertexId);
            CheckCompileErrors(m_vertexId, "Vertex");
            glAttachShader(m_id, m_vertexId);
            break;
        case ShaderType::FRAGMENT:
            if (m_fragmentId != 0) glDeleteShader(m_fragmentId);
            m_fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(m_fragmentId, 1, &c_code, nullptr);
            glCompileShader(m_fragmentId);
            CheckCompileErrors(m_fragmentId, "Fragment");
            glAttachShader(m_id, m_fragmentId);
            break;
        case ShaderType::GEOMETRY:
            if (m_geometryId != 0) glDeleteShader(m_geometryId);
            m_geometryId = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(m_geometryId, 1, &c_code, nullptr);
            glCompileShader(m_geometryId);
            CheckCompileErrors(m_geometryId, "Geometry");
            glAttachShader(m_id, m_geometryId);
            break;
        case ShaderType::COMPUTE:
            if (m_computeId != 0) glDeleteShader(m_computeId);
            m_computeId = glCreateShader(GL_COMPUTE_SHADER);
            glShaderSource(m_computeId, 1, &c_code, nullptr);
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

void GLShader::SetMat3(const std::string& name, const glm::mat3& value) {
    glProgramUniformMatrix3fv(m_id, glGetUniformLocation(m_id, name.c_str()), 1,
                              GL_FALSE, glm::value_ptr(value));
}

void GLShader::SetMat4(const std::string& name, const glm::mat4& value) {
    glProgramUniformMatrix4fv(m_id, glGetUniformLocation(m_id, name.c_str()), 1,
                              GL_FALSE, glm::value_ptr(value));
}

void GLShader::SetTexture(const std::string& name, const Texture* texture) {
    uint8_t id = glGetUniformLocation(m_id, name.c_str()) % MAX_TEXTURES;
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

uint32_t GLShader::GetUint(const std::string& name) const {
    uint32_t value = 0;
    glGetUniformuiv(m_id, glGetUniformLocation(m_id, name.c_str()), &value);
    return value;
}

}  // namespace SD
