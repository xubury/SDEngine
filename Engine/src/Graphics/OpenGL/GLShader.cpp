#include "Graphics/OpenGL/GLShader.hpp"
#include "Graphics/OpenGL/GLBuffer.hpp"
#include "Graphics/OpenGL/GLTexture.hpp"
#include "Utility/Log.hpp"
#include "Utility/Assert.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>

namespace sd {

GLShader::GLShader()
    : m_id(0), m_vertexId(0), m_fragmentId(0), m_geometryId(0), m_computeId(0) {
    m_id = glCreateProgram();
}

GLShader::~GLShader() {
    glDeleteProgram(m_id);
    destroyShaders();
}

void GLShader::compileShader(ShaderType type, const char* code) {
    switch (type) {
        case ShaderType::VERTEX:
            if (m_vertexId != 0) glDeleteShader(m_vertexId);
            m_vertexId = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(m_vertexId, 1, &code, nullptr);
            glCompileShader(m_vertexId);
            checkCompileErrors(m_vertexId, "Vertex");
            glAttachShader(m_id, m_vertexId);
            break;
        case ShaderType::FRAGMENT:
            if (m_fragmentId != 0) glDeleteShader(m_fragmentId);
            m_fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(m_fragmentId, 1, &code, nullptr);
            glCompileShader(m_fragmentId);
            checkCompileErrors(m_fragmentId, "Fragment");
            glAttachShader(m_id, m_fragmentId);
            break;
        case ShaderType::GEOMETRY:
            if (m_geometryId != 0) glDeleteShader(m_geometryId);
            m_geometryId = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(m_geometryId, 1, &code, nullptr);
            glCompileShader(m_geometryId);
            checkCompileErrors(m_geometryId, "Geometry");
            glAttachShader(m_id, m_geometryId);
            break;
        case ShaderType::COMPUTE:
            if (m_computeId != 0) glDeleteShader(m_computeId);
            m_computeId = glCreateShader(GL_COMPUTE_SHADER);
            glShaderSource(m_computeId, 1, &code, nullptr);
            glCompileShader(m_computeId);
            checkCompileErrors(m_computeId, "Compute");
            glAttachShader(m_id, m_computeId);
            break;
        case ShaderType::INVALID:
            SD_CORE_ERROR("Trying to add a invalid shader");
            break;
    }
}

void GLShader::destroyShaders() {
    if (m_vertexId != 0) glDeleteShader(m_vertexId);
    if (m_fragmentId != 0) glDeleteShader(m_fragmentId);
    if (m_geometryId != 0) glDeleteShader(m_geometryId);
    if (m_computeId != 0) glDeleteShader(m_computeId);
    m_vertexId = 0;
    m_fragmentId = 0;
    m_geometryId = 0;
    m_computeId = 0;
}

void GLShader::linkShaders() {
    glLinkProgram(m_id);
    checkCompileErrors(m_id, "Program");
    destroyShaders();
}

void GLShader::bind() { glUseProgram(m_id); }

void GLShader::checkCompileErrors(uint32_t shader, const std::string& type) {
    int success;
    int logSize = 0;
    std::string infoLog;
    if (type != "Program") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
        infoLog.resize(logSize);
        if (!success) {
            glGetShaderInfoLog(shader, logSize, nullptr, infoLog.data());
            SD_CORE_ERROR("{} Shader compilation error:\n{}", type, infoLog);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &logSize);
        infoLog.resize(logSize);
        if (!success) {
            glGetProgramInfoLog(shader, logSize, nullptr, infoLog.data());
            SD_CORE_ERROR("Shader Program linking error:\n{}", infoLog);
        }
    }
    SD_CORE_ASSERT(success);
}

void GLShader::setBool(const std::string& name, bool value) {
    glProgramUniform1i(m_id, glGetUniformLocation(m_id, name.c_str()), value);
}

void GLShader::setInt(const std::string& name, int value) {
    glProgramUniform1i(m_id, glGetUniformLocation(m_id, name.c_str()), value);
}

void GLShader::setUint(const std::string& name, uint32_t value) {
    glProgramUniform1ui(m_id, glGetUniformLocation(m_id, name.c_str()), value);
}

void GLShader::setFloat(const std::string& name, float value) {
    glProgramUniform1f(m_id, glGetUniformLocation(m_id, name.c_str()), value);
}

void GLShader::setVec2(const std::string& name, const glm::vec2& value) {
    glProgramUniform2fv(m_id, glGetUniformLocation(m_id, name.c_str()), 1,
                        &value[0]);
}

void GLShader::setVec3(const std::string& name, const glm::vec3& value) {
    glProgramUniform3fv(m_id, glGetUniformLocation(m_id, name.c_str()), 1,
                        &value[0]);
}

void GLShader::setVec4(const std::string& name, const glm::vec4& value) {
    glProgramUniform4fv(m_id, glGetUniformLocation(m_id, name.c_str()), 1,
                        &value[0]);
}

void GLShader::setMat4(const std::string& name, const glm::mat4& value) {
    glProgramUniformMatrix4fv(m_id, glGetUniformLocation(m_id, name.c_str()), 1,
                              GL_FALSE, glm::value_ptr(value));
}

void GLShader::setTexture(const std::string& name, const Texture* texture) {
    uint8_t id = glGetUniformLocation(m_id, name.c_str());
    id = id % 32;
    setInt(name, id);
    setTexture(id, texture);
}

void GLShader::setTexture(uint32_t id, const Texture* texture) {
    if (texture) {
        texture->setSlot(id);
    } else {
        glBindTextureUnit(id, 0);
    }
}

void GLShader::setUniformBuffer(const std::string& name,
                                const UniformBuffer& buffer) {
    buffer.bind();
    uint32_t index = glGetUniformBlockIndex(m_id, name.c_str());
    if (index != GL_INVALID_INDEX)
        glUniformBlockBinding(m_id, index, buffer.getBindingPoint());
}

}  // namespace sd
