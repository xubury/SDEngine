#ifndef SD_GL_SHADER_HPP
#define SD_GL_SHADER_HPP

#include "Utility/Base.hpp"
#include "Utility/Export.hpp"
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

    void bind() override;

    void unbind() override;

    void setBool(const std::string& name, bool value) override;

    void setInt(const std::string& name, int value) override;

    void setUint(const std::string& name, uint32_t value) override;

    void setFloat(const std::string& name, float value) override;

    void setVec2(const std::string& name, const glm::vec2& value) override;
    void setVec3(const std::string& name, const glm::vec3& value) override;
    void setVec4(const std::string& name, const glm::vec4& value) override;
    void setMat4(const std::string& name, const glm::mat4& value) override;

    void setTexture(const std::string& name, const Texture* texture) override;
    void setTexture(uint32_t id, const Texture* texture) override;

    void setUniformBuffer(const std::string& name,
                          const UniformBuffer& buffer) override;

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
