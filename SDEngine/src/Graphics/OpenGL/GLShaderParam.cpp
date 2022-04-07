#include "Graphics/OpenGL/GLShaderParam.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"

#include <GL/glew.h>

namespace SD {

GLShaderParam::GLShaderParam(UniformType type, const std::string& name,
                             int32_t index, uint32_t program_id,
                             int32_t location, int32_t tex_binding_id,
                             int32_t image_binding)
    : ShaderParam(type, name, index),
      m_program(program_id),
      m_location(location),
      m_texture_binding(tex_binding_id),
      m_image_binding(image_binding)
{
}

void GLShaderParam::SetAsBool(bool value)
{
    glProgramUniform1i(m_program, m_location, value);
}

void GLShaderParam::SetAsInt(int value)
{
    glProgramUniform1i(m_program, m_location, value);
}

void GLShaderParam::SetAsUint(uint32_t value)
{
    glProgramUniform1ui(m_program, m_location, value);
}

void GLShaderParam::SetAsFloat(float value)
{
    glProgramUniform1f(m_program, m_location, value);
}

void GLShaderParam::SetAsVec(const int* value, int32_t count)
{
    glProgramUniform1iv(m_program, m_location, count, value);
}

void GLShaderParam::SetAsVec(const float* value, int32_t count)
{
    glProgramUniform1fv(m_program, m_location, count, value);
}

void GLShaderParam::SetAsVec2(const float* value, int32_t count)
{
    glProgramUniform2fv(m_program, m_location, count, value);
}

void GLShaderParam::SetAsVec3(const float* value, int32_t count)
{
    glProgramUniform3fv(m_program, m_location, count, value);
}

void GLShaderParam::SetAsVec4(const float* value, int32_t count)
{
    glProgramUniform4fv(m_program, m_location, count, value);
}

void GLShaderParam::SetAsMat3(const float* value, int32_t count)
{
    glProgramUniformMatrix3fv(m_program, m_location, count, GL_FALSE, value);
}

void GLShaderParam::SetAsMat4(const float* value, int32_t count)
{
    glProgramUniformMatrix4fv(m_program, m_location, count, GL_FALSE, value);
}

void GLShaderParam::SetAsTexture(const Texture* texture)
{
    SetAsInt(m_texture_binding);
    glBindTextureUnit(m_texture_binding, texture ? texture->GetId() : 0);
}

void GLShaderParam::SetAsTextures(const Texture** textures, int32_t count)
{
    std::vector<int32_t> bindings(count);
    for (int32_t i = 0; i < count; ++i) {
        bindings[i] = m_texture_binding + i;
        glBindTextureUnit(bindings[i], textures[i] ? textures[i]->GetId() : 0);
    }
    SetAsVec(&bindings[0], count);
}

void GLShaderParam::SetAsImage(const Texture* texture, int32_t level,
                               bool layered, int32_t layer, Access access)
{
    SetAsInt(m_image_binding);
    glBindImageTexture(m_image_binding, texture ? texture->GetId() : 0, level,
                       layered, layer, Translate(access),
                       Translate(texture->GetFormat()));
}

}  // namespace SD
