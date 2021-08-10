#ifndef SD_GL_SHADER_HPP
#define SD_GL_SHADER_HPP

#include "Common/Base.hpp"
#include "Common/Export.hpp"
#include "Graphics/Shader.hpp"
#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>
#include <unordered_map>

namespace sd {

class UniformBuffer;

class SD_API GLShader : public Shader {
   public:
    GLShader();

    ~GLShader();

    void init() override;

    void compileShader(ShaderType type, const char* code) override;

    void linkShaders() override;

    void bind() const override;

    void unbind() const override;

    void setBool(const std::string& name, bool value) const override;

    void setInt(const std::string& name, int value) const override;

    void setFloat(const std::string& name, float value) const override;

    void setVec2(const std::string& name,
                 const glm::vec2& value) const override;
    void setVec3(const std::string& name,
                 const glm::vec3& value) const override;
    void setVec4(const std::string& name,
                 const glm::vec4& value) const override;
    void setMat4(const std::string& name,
                 const glm::mat4& value) const override;

    void setTexture(const Texture& texture, int index) const override;

    void setUniformBuffer(const std::string& name,
                          const UniformBuffer& buffer) const override;

   private:
    static void checkCompileErrors(uint32_t shader, const std::string& type);

    void destroyShaders();

    GLuint m_id;
    GLuint m_vertexId;
    GLuint m_fragmentId;
    GLuint m_geometryId;
    GLuint m_computeId;
};

}  // namespace sd

#endif /* SD_GL_SHADER_HPP */
