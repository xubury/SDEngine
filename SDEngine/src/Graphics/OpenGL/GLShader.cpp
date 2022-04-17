#include "Graphics/OpenGL/GLShader.hpp"
#include "Graphics/OpenGL/GLBuffer.hpp"
#include "Graphics/OpenGL/GLTexture.hpp"
#include "Graphics/OpenGL/GLShaderParam.hpp"

namespace SD {

UniformType GetUniformType(GLenum gl_type)
{
    switch (gl_type) {
        case GL_BOOL_VEC2:
        case GL_BOOL_VEC3:
        case GL_BOOL_VEC4:
        case GL_BOOL:
            return UniformType::Bool;

        case GL_INT_VEC2:
        case GL_INT_VEC3:
        case GL_INT_VEC4:
        case GL_INT:
            return UniformType::Int;

        case GL_UNSIGNED_INT_VEC2:
        case GL_UNSIGNED_INT_VEC3:
        case GL_UNSIGNED_INT_VEC4:
        case GL_UNSIGNED_INT:
            return UniformType::Int;

        case GL_FLOAT_MAT2:
        case GL_FLOAT_MAT2x3:
        case GL_FLOAT_MAT2x4:
        case GL_FLOAT_MAT3:
        case GL_FLOAT_MAT3x2:
        case GL_FLOAT_MAT3x4:
        case GL_FLOAT_MAT4:
        case GL_FLOAT_MAT4x2:
        case GL_FLOAT_MAT4x3:
        case GL_FLOAT_VEC2:
        case GL_FLOAT_VEC3:
        case GL_FLOAT_VEC4:
        case GL_FLOAT:
            return UniformType::Float;

        case GL_DOUBLE_MAT2:
        case GL_DOUBLE_MAT2x3:
        case GL_DOUBLE_MAT2x4:
        case GL_DOUBLE_MAT3:
        case GL_DOUBLE_MAT3x2:
        case GL_DOUBLE_MAT3x4:
        case GL_DOUBLE_MAT4:
        case GL_DOUBLE_MAT4x2:
        case GL_DOUBLE_MAT4x3:
        case GL_DOUBLE_VEC2:
        case GL_DOUBLE_VEC3:
        case GL_DOUBLE_VEC4:
        case GL_DOUBLE:
            return UniformType::Double;

        case GL_SAMPLER_2D:
        case GL_SAMPLER_2D_ARRAY:
        case GL_SAMPLER_CUBE:
        case GL_SAMPLER_2D_MULTISAMPLE:
        case GL_SAMPLER_3D:
            return UniformType::Sampler;

        case GL_IMAGE_2D:
        case GL_IMAGE_2D_ARRAY:
        case GL_IMAGE_CUBE:
        case GL_IMAGE_2D_MULTISAMPLE:
        case GL_IMAGE_3D:
            return UniformType::Image;

        default:
            return UniformType::Unknown;
    }
}

GLShader::GLShader()
    : m_vertexId(0),
      m_fragmentId(0),
      m_geometryId(0),
      m_computeId(0),
      m_texture_cnt(0),
      m_image_cnt(0)
{
    m_id = glCreateProgram();
}

GLShader::~GLShader()
{
    glDeleteProgram(m_id);
    DestroyShaders();

    for (auto& [_, param] : m_params) {
        delete param;
    }
}

int32_t GLShader::GetUniformCount() const
{
    int32_t count;
    glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &count);
    return count;
}

int32_t GLShader::GetUniformBufferCount() const
{
    int32_t count;
    glGetProgramiv(m_id, GL_ACTIVE_UNIFORM_BLOCKS, &count);
    return count;
}

ShaderParam* GLShader::GetParam(const std::string& name)
{
    auto itr = m_params.find(name);
    if (itr != m_params.end()) {
        return itr->second;
    }

    SD_CORE_ASSERT(
        false,
        fmt::format("No shader uniform variable corresponding to: \"{}\"",
                    name));
    return nullptr;
}

ShaderParam* GLShader::GetParam(int32_t index)
{
    int32_t name_size;
    int32_t size;
    GLenum gl_type;
    char name[128];
    memset(name, '\0', 128);

    glGetActiveUniform(m_id, index, 128, &name_size, &size, &gl_type, name);
    int32_t location = glGetUniformLocation(m_id, name);

    if (location == -1) {
        // glGetActiveUniform returns -1 if name does not correspond to an
        // active uniform variable in program, if name starts with the reserved
        // prefix "gl_", or if name is associated with an atomic counter or a
        // named uniform block
        return nullptr;
    }
    std::string s(name);

    auto itr = m_params.find(s);
    if (itr != m_params.end()) {
        return itr->second;
    }

    UniformType type = GetUniformType(gl_type);
    if (type == UniformType::Unknown) {
        SD_CORE_ERROR("Unknown shader uniform type:{} for uniform:\"{}\"!",
                      gl_type, s);
        return nullptr;
    }
    int32_t tex_binding_id = -1;
    int32_t image_binding = -1;

    if (type == UniformType::Sampler) {
        tex_binding_id = m_texture_cnt;
        m_texture_cnt += size;
    }
    else if (type == UniformType::Image) {
        image_binding = m_image_cnt;
        m_image_cnt += size;
    }
    ShaderParam* p = new GLShaderParam(type, s, index, m_id, location,
                                       tex_binding_id, image_binding, size);

    return m_params.emplace(s, p).first->second;
}

void GLShader::CompileShader(ShaderType type, const std::string& code)
{
    const char* c_code = code.c_str();
    switch (type) {
        case ShaderType::Vertex:
            if (m_vertexId != 0) glDeleteShader(m_vertexId);
            m_vertexId = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(m_vertexId, 1, &c_code, nullptr);
            glCompileShader(m_vertexId);
            CheckCompileErrors(m_vertexId, "Vertex");
            glAttachShader(m_id, m_vertexId);
            break;
        case ShaderType::Fragment:
            if (m_fragmentId != 0) glDeleteShader(m_fragmentId);
            m_fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(m_fragmentId, 1, &c_code, nullptr);
            glCompileShader(m_fragmentId);
            CheckCompileErrors(m_fragmentId, "Fragment");
            glAttachShader(m_id, m_fragmentId);
            break;
        case ShaderType::Geometry:
            if (m_geometryId != 0) glDeleteShader(m_geometryId);
            m_geometryId = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(m_geometryId, 1, &c_code, nullptr);
            glCompileShader(m_geometryId);
            CheckCompileErrors(m_geometryId, "Geometry");
            glAttachShader(m_id, m_geometryId);
            break;
        case ShaderType::Compute:
            if (m_computeId != 0) glDeleteShader(m_computeId);
            m_computeId = glCreateShader(GL_COMPUTE_SHADER);
            glShaderSource(m_computeId, 1, &c_code, nullptr);
            glCompileShader(m_computeId);
            CheckCompileErrors(m_computeId, "Compute");
            glAttachShader(m_id, m_computeId);
            break;
        case ShaderType::Invalid:
            SD_CORE_ERROR("Trying to add a invalid shader");
            break;
    }
}

void GLShader::DestroyShaders()
{
    if (m_vertexId != 0) glDeleteShader(m_vertexId);
    if (m_fragmentId != 0) glDeleteShader(m_fragmentId);
    if (m_geometryId != 0) glDeleteShader(m_geometryId);
    if (m_computeId != 0) glDeleteShader(m_computeId);
    m_vertexId = 0;
    m_fragmentId = 0;
    m_geometryId = 0;
    m_computeId = 0;
}

void GLShader::LinkShaders()
{
    glLinkProgram(m_id);
    CheckCompileErrors(m_id, "Program");
    DestroyShaders();

    int32_t uniform_cnt = GetUniformCount();
    // Getting all uniform parameter
    for (int32_t i = 0; i < uniform_cnt; ++i) {
        GetParam(i);
    }
}

void GLShader::CheckCompileErrors(uint32_t shader, const std::string& type)
{
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
    }
    else {
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

void GLShader::SetUniformBuffer(const std::string& name,
                                const UniformBuffer& buffer)
{
    uint32_t index = glGetUniformBlockIndex(m_id, name.c_str());
    if (index != GL_INVALID_INDEX) {
        glUniformBlockBinding(m_id, index, buffer.GetBindingPoint());
    }
}

uint32_t GLShader::GetUint(const std::string& name) const
{
    uint32_t value = 0;
    glGetUniformuiv(m_id, glGetUniformLocation(m_id, name.c_str()), &value);
    return value;
}

Vector3i GLShader::GetLocalGroupSize() const
{
    Vector3i group_size;
    glGetProgramiv(m_id, GL_COMPUTE_WORK_GROUP_SIZE, &group_size[0]);
    return group_size;
}

}  // namespace SD
