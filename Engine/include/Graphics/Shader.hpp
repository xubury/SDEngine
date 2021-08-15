#ifndef SD_SHADER_HPP
#define SD_SHADER_HPP

#include "Common/Base.hpp"
#include "Common/Export.hpp"
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace sd {

class Texture;
class UniformBuffer;

enum class ShaderType { INVALID, VERTEX, FRAGMENT, GEOMETRY, COMPUTE };

class SD_API Shader {
   public:
    static Ref<Shader> create();

    virtual ~Shader() = default;

    Shader(const Shader&) = delete;

    Shader& operator=(const Shader&) = delete;

    virtual void init() = 0;

    virtual void compileShader(ShaderType type, const char* code) = 0;

    virtual void linkShaders() = 0;

    virtual void bind() const = 0;

    virtual void unbind() const = 0;

    virtual void setBool(const std::string& name, bool value) const = 0;

    virtual void setInt(const std::string& name, int value) const = 0;

    virtual void setUint(const std::string& name, uint32_t value) const = 0;

    virtual void setFloat(const std::string& name, float value) const = 0;

    virtual void setVec2(const std::string& name,
                         const glm::vec2& value) const = 0;

    virtual void setVec3(const std::string& name,
                         const glm::vec3& value) const = 0;

    virtual void setVec4(const std::string& name,
                         const glm::vec4& value) const = 0;

    virtual void setMat4(const std::string& name,
                         const glm::mat4& value) const = 0;

    virtual void setTexture(const Texture& texture, int index) const = 0;

    virtual void setUniformBuffer(const std::string& name,
                                  const UniformBuffer& buffer) const = 0;

   protected:
    Shader() = default;
};

}  // namespace sd

#endif /* SD_SHADER_HPP */
