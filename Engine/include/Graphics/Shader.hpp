#ifndef SD_SHADER_HPP
#define SD_SHADER_HPP

#include "Utils/Base.hpp"
#include "Utils/Export.hpp"
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

    virtual void bind() = 0;

    virtual void unbind() = 0;

    virtual void setBool(const std::string& name, bool value) = 0;

    virtual void setInt(const std::string& name, int value) = 0;

    virtual void setUint(const std::string& name, uint32_t value) = 0;

    virtual void setFloat(const std::string& name, float value) = 0;

    virtual void setVec2(const std::string& name, const glm::vec2& value) = 0;

    virtual void setVec3(const std::string& name, const glm::vec3& value) = 0;

    virtual void setVec4(const std::string& name, const glm::vec4& value) = 0;

    virtual void setMat4(const std::string& name, const glm::mat4& value) = 0;

    virtual void setTexture(const std::string& name,
                            const Texture* texture) = 0;

    virtual void setUniformBuffer(const std::string& name,
                                  const UniformBuffer& buffer) = 0;

   protected:
    Shader() = default;
};

}  // namespace sd

#endif /* SD_SHADER_HPP */
