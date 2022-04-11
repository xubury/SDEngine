#ifndef SD_SHADER_HPP
#define SD_SHADER_HPP

#include "Utility/Base.hpp"
#include "Graphics/Export.hpp"
#include "Graphics/ShaderParam.hpp"
#include "Utility/Math.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

namespace SD {

class Texture;
class UniformBuffer;

enum class ShaderType { Invalid, Vertex, Fragment, Geometry, Compute };

class SD_GRAPHICS_API Shader {
   public:
    static Ref<Shader> Create();

    virtual ~Shader() = default;

    virtual uint32_t Handle() const = 0;
    virtual ShaderParam* GetParam(int32_t index) = 0;
    virtual ShaderParam* GetParam(const std::string& name) = 0;

    Shader(const Shader&) = delete;

    Shader& operator=(const Shader&) = delete;

    virtual void CompileShader(ShaderType type, const std::string& code) = 0;

    virtual void LinkShaders() = 0;

    virtual void SetUniformBuffer(const std::string& name,
                                  const UniformBuffer& buffer) = 0;

    virtual uint32_t GetUint(const std::string& name) const = 0;

    virtual Vector3i GetLocalGroupSize() const = 0;

   protected:
    Shader() = default;
};

}  // namespace SD

#endif /* SD_SHADER_HPP */
