#ifndef SD_SHADER_HPP
#define SD_SHADER_HPP

#include "Utility/Base.hpp"
#include "Graphics/Export.hpp"
#include "Graphics/Resource.hpp"

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <unordered_map>
#include <filesystem>

namespace SD {

class Texture;
class UniformBuffer;

enum class ShaderType { INVALID, VERTEX, FRAGMENT, GEOMETRY, COMPUTE };

class SD_GRAPHICS_API Shader : public Resource {
   public:
    static Ref<Shader> Create();

    virtual ~Shader() = default;

    Shader(const Shader&) = delete;

    Shader& operator=(const Shader&) = delete;

    virtual void CompileShader(ShaderType type, const std::string& code) = 0;

    virtual void LinkShaders() = 0;

    virtual void SetBool(const std::string& name, bool value) = 0;

    virtual void SetInt(const std::string& name, int value) = 0;

    virtual void SetUint(const std::string& name, uint32_t value) = 0;

    virtual void SetFloat(const std::string& name, float value) = 0;

    virtual void SetVec2(const std::string& name, const glm::vec2& value) = 0;

    virtual void SetVec3(const std::string& name, const glm::vec3& value) = 0;

    virtual void SetVec4(const std::string& name, const glm::vec4& value) = 0;

    virtual void SetMat3(const std::string& name, const glm::mat3& value) = 0;

    virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

    virtual void SetTexture(const std::string& name,
                            const Texture* texture) = 0;
    virtual void SetTexture(const uint32_t id, const Texture* texture) = 0;

    virtual void SetUniformBuffer(const std::string& name,
                                  const UniformBuffer& buffer) = 0;

    virtual uint32_t GetUint(const std::string& name) const = 0;

   protected:
    Shader() = default;
};

}  // namespace SD

#endif /* SD_SHADER_HPP */
