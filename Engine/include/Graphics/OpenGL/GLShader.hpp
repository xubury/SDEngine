#ifndef SD_GL_SHADER_HPP
#define SD_GL_SHADER_HPP

#include "Utility/Base.hpp"
#include "Utility/Export.hpp"
#include "Graphics/Shader.hpp"
#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>
#include <unordered_map>

namespace SD {

class UniformBuffer;

class SD_API GLShader : public Shader {
   public:
    GLShader(const std::string &filePath);

    ~GLShader();

    void CompileShader(ShaderType type, const char* code) override;

    void LinkShaders() override;

    void Bind() override;

    void SetBool(const std::string& name, bool value) override;

    void SetInt(const std::string& name, int value) override;

    void SetUint(const std::string& name, uint32_t value) override;

    void SetFloat(const std::string& name, float value) override;

    void SetVec2(const std::string& name, const glm::vec2& value) override;
    void SetVec3(const std::string& name, const glm::vec3& value) override;
    void SetVec4(const std::string& name, const glm::vec4& value) override;
    void SetMat4(const std::string& name, const glm::mat4& value) override;

    void SetTexture(const std::string& name, const Texture* texture) override;
    void SetTexture(uint32_t id, const Texture* texture) override;

    void SetUniformBuffer(const std::string& name,
                          const UniformBuffer& buffer) override;

   private:
    static void CheckCompileErrors(uint32_t shader, const std::string& type);

    void DestroyShaders();

    GLuint m_id;
    GLuint m_vertexId;
    GLuint m_fragmentId;
    GLuint m_geometryId;
    GLuint m_computeId;
};

}  // namespace SD

#endif /* SD_GL_SHADER_HPP */
