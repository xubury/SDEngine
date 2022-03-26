#ifndef SD_GL_SHADER_HPP
#define SD_GL_SHADER_HPP

#include "Utility/Base.hpp"
#include "Utility/Export.hpp"
#include "Graphics/Shader.hpp"
#include <GL/glew.h>
#include <string>
#include <unordered_map>

namespace SD {

class UniformBuffer;

class SD_GRAPHICS_API GLShader : public Shader {
   public:
    GLShader();

    ~GLShader();

    uint32_t GetId() const override { return m_id; }

    ShaderParam* GetParam(int32_t index) override;
    ShaderParam* GetParam(const std::string& name) override;

    void CompileShader(ShaderType type, const std::string& code) override;

    void LinkShaders() override;

    void SetUniformBuffer(const std::string& name,
                          const UniformBuffer& buffer) override;

    uint32_t GetUint(const std::string& name) const override;

   private:
    static void CheckCompileErrors(uint32_t shader, const std::string& type);

    void DestroyShaders();

    int32_t GetUniformCount() const;
    int32_t GetUniformBufferCount() const;

    GLuint m_id;
    GLuint m_vertexId;
    GLuint m_fragmentId;
    GLuint m_geometryId;
    GLuint m_computeId;

    std::unordered_map<std::string, ShaderParam*> m_params;
    uint32_t m_texture_cnt;
};

}  // namespace SD

#endif /* SD_GL_SHADER_HPP */
