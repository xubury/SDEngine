#include "Graphics/OpenGL/GLShader.hpp"
#include "Graphics/OpenGL/GLBuffer.hpp"
#include "Graphics/OpenGL/GLTexture.hpp"
#include "Utility/Log.hpp"
#include "Utility/Assert.hpp"
#include "Utility/File.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace sd {

static ShaderType shaderTypeFromName(const std::string& name) {
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
    File::read(filePath, source);

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
            std::filesystem::path include =
                code.substr(offset, start - offset - 1);

            code.erase(j, start - j);
            std::string includeCode;
            File::read(("assets" / include).string(), includeCode);
            code.insert(j, includeCode);

            j = code.find("#include", start);
        }

        compileShader(type, code.c_str());
    }
    SD_CORE_ASSERT(success, "Error building shader code");
    linkShaders();
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
