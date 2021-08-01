#include "Graphics/OpenGL/GLShader.hpp"
#include "Graphics/OpenGL/GLBuffer.hpp"
#include "Graphics/OpenGL/GLTexture.hpp"
#include "Utils/Log.hpp"
#include "Utils/Assert.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>

namespace sd {

static void fileToString(const std::string& path, std::string& string) {
    std::ifstream file;
    // ensure ifstream objects can throw exceptions:
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // open files
        file.open(path);
        std::stringstream fileStream;
        // read file's buffer contents into streams
        fileStream << file.rdbuf();
        // close file handlers
        file.close();
        // convert stream into string
        string = fileStream.str();

        size_t i = string.find("#include");
        while (i < string.size()) {
            size_t start = string.find('\n', i) + 1;
            size_t offset = i + 9;
            std::string includePath = string.substr(offset, start - offset - 1);
            std::string include;
            fileToString(includePath, include);
            string.erase(i - 1, start - i);
            string.insert(i, include);
            i = string.find("#incldue");
        }
    } catch (std::ifstream::failure& e) {
        SD_CORE_ERROR("File not successfully read!");
    }
}

GLShader::GLShader() : m_id(0) {}

void GLShader::loadFromFile(const std::string& vertexPath,
                            const std::string& fragmentPath,
                            const std::string& geometryPath) {
    std::string vertexCode;
    std::string fragmentCode;
    fileToString(vertexPath, vertexCode);
    fileToString(fragmentPath, fragmentCode);
    if (!geometryPath.empty()) {
        std::string geometryCode;
        fileToString(geometryPath, geometryCode);
        compile(vertexCode.c_str(), fragmentCode.c_str(), geometryCode.c_str());
    }
    compile(vertexCode.c_str(), fragmentCode.c_str());
}

void GLShader::compile(const char* vertexCode, const char* fragmentCode,
                       const char* geometryCode) {
    // compile shaders
    // vertex shader
    uint32_t vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "Vertex");
    // fragment Shader
    uint32_t fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentCode, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "Fragment");

    uint32_t geometry = 0;
    if (geometryCode != nullptr) {
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &geometryCode, nullptr);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "Geometry");
    }
    // shader Program
    m_id = glCreateProgram();
    glAttachShader(m_id, fragment);
    glAttachShader(m_id, vertex);
    if (geometry != 0) glAttachShader(m_id, geometry);

    glLinkProgram(m_id);
    checkCompileErrors(m_id, "Program");
    // delete the shaders as they're linked into our program now and no longer
    // necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometry != 0) glDeleteShader(geometry);
}

void GLShader::bind() const { glUseProgram(m_id); }

void GLShader::unbind() const { glUseProgram(0); }

void GLShader::checkCompileErrors(uint32_t shader, const std::string& type) {
    int success;
    std::string infoLog(1024, 0);
    if (type != "Program") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog.data());
            SD_CORE_ERROR("{0} Shader compilation error:\n{1}\n", type,
                          infoLog);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog.data());
            SD_CORE_ERROR("Shader Program linking error:\n{0}\n", infoLog);
        }
    }
}

void GLShader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void GLShader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void GLShader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void GLShader::setVec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void GLShader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void GLShader::setVec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void GLShader::setMat4(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE,
                       glm::value_ptr(value));
}

void GLShader::setTexture(const std::string& name, const Texture& texture,
                          int index) const {
    setInt(name, index);
    glActiveTexture(GL_TEXTURE0 + index);
    texture.bind();
}

void GLShader::setUniformBuffer(const std::string& name,
                                const UniformBuffer& buffer) const {
    buffer.bind();
    uint32_t index = glGetUniformBlockIndex(m_id, name.c_str());
    if (index != GL_INVALID_INDEX)
        glUniformBlockBinding(m_id, index, buffer.getBindingPoint());
}

}  // namespace sd
