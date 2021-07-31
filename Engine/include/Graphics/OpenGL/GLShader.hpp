#ifndef SD_GL_SHADER_HPP
#define SD_GL_SHADER_HPP

#include "Core/Base.hpp"
#include "Core/Export.hpp"
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

    virtual ~GLShader() = default;

    void compile(const char* vertexCode, const char* fragmentCode,
                 const char* geometryCode = nullptr) override;

    void loadFromFile(const std::string& vertexPath,
                      const std::string& fragmentPath,
                      const std::string& geometryPath = "") override;

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

    void setUniformBuffer(const std::string& name,
                          const UniformBuffer& buffer) const override;

   private:
    static void checkCompileErrors(uint32_t shader, const std::string& type);

    GLuint m_id;
};

}  // namespace sd

#endif /* SD_GL_SHADER_HPP */
