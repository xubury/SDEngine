#include "Graphics/OpenGL/GLShaderParam.hpp"

#include <GL/glew.h>

namespace SD {

GLShaderParam::GLShaderParam(UniformType type, const std::string& name,
                             int32_t index, uint32_t program_id,
                             int32_t location, int32_t tex_binding_id)
    : ShaderParam(type, name, index),
      m_program(program_id),
      m_location(location),
      m_tex_binding_id(tex_binding_id) {}

void GLShaderParam::SetAsBool(bool value) {
    glProgramUniform1i(m_program, m_location, value);
}

void GLShaderParam::SetAsInt(int value) {
    glProgramUniform1i(m_program, m_location, value);
}

void GLShaderParam::SetAsUint(uint32_t value) {
    glProgramUniform1ui(m_program, m_location, value);
}

void GLShaderParam::SetAsFloat(float value) {
    glProgramUniform1f(m_program, m_location, value);
}

void GLShaderParam::SetAsVec(const int* value, int32_t count) {
    glProgramUniform1iv(m_program, m_location, count, value);
}

void GLShaderParam::SetAsVec(const float* value, int32_t count) {
    glProgramUniform1fv(m_program, m_location, count, value);
}

void GLShaderParam::SetAsVec2(const float* value, int32_t count) {
    glProgramUniform2fv(m_program, m_location, count, value);
}

void GLShaderParam::SetAsVec3(const float* value, int32_t count) {
    glProgramUniform3fv(m_program, m_location, count, value);
}

void GLShaderParam::SetAsVec4(const float* value, int32_t count) {
    glProgramUniform4fv(m_program, m_location, count, value);
}

void GLShaderParam::SetAsMat3(const float* value, int32_t count) {
    glProgramUniformMatrix3fv(m_program, m_location, count, GL_FALSE, value);
}

void GLShaderParam::SetAsMat4(const float* value, int32_t count) {
    glProgramUniformMatrix4fv(m_program, m_location, count, GL_FALSE, value);
}

void GLShaderParam::SetAsTexture(const Texture* texture) {
    SetAsInt(m_tex_binding_id);
    if (texture) {
        glBindTextureUnit(m_tex_binding_id, texture->GetId());
    } else {
        glBindTextureUnit(m_tex_binding_id, 0);
    }
}

void GLShaderParam::SetAsTextures(const Texture** textures, int32_t count) {
    std::vector<int32_t> bindings(count);
    for (int32_t i = 0; i < count; ++i) {
        bindings[i] = m_tex_binding_id + i;
        if (textures[i]) {
            glBindTextureUnit(bindings[i], textures[i]->GetId());
        } else {
            glBindTextureUnit(bindings[i], 0);
        }
    }
    SetAsVec(&bindings[0], count);
}

}  // namespace SD
