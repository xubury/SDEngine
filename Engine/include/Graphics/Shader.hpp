#ifndef SD_SHADER_HPP
#define SD_SHADER_HPP

#include "Utility/Base.hpp"

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <unordered_map>
#include <filesystem>

namespace SD {

class Texture;
class UniformBuffer;

enum class ShaderType { INVALID, VERTEX, FRAGMENT, GEOMETRY, COMPUTE };

class SD_API Shader {
   public:
    static Ref<Shader> Create(const std::string& filePath);

    virtual ~Shader() = default;

    Shader(const Shader&) = delete;

    Shader& operator=(const Shader&) = delete;

    virtual void CompileShader(ShaderType type, const char* code) = 0;

    virtual void LinkShaders() = 0;

    virtual void Bind() = 0;

    virtual void SetBool(const std::string& name, bool value) = 0;

    virtual void SetInt(const std::string& name, int value) = 0;

    virtual void SetUint(const std::string& name, uint32_t value) = 0;

    virtual void SetFloat(const std::string& name, float value) = 0;

    virtual void SetVec2(const std::string& name, const glm::vec2& value) = 0;

    virtual void SetVec3(const std::string& name, const glm::vec3& value) = 0;

    virtual void SetVec4(const std::string& name, const glm::vec4& value) = 0;

    virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

    virtual void SetTexture(const std::string& name,
                            const Texture* texture) = 0;
    virtual void SetTexture(const uint32_t id, const Texture* texture) = 0;

    virtual void SetUniformBuffer(const std::string& name,
                                  const UniformBuffer& buffer) = 0;

   protected:
    Shader() = default;
};

class ShaderLibrary {
   public:
    static ShaderLibrary& Instance() {
        static ShaderLibrary s_instance;
        return s_instance;
    }
    void Add(const std::string& name, const Ref<Shader>& shader);
    Ref<Shader> Load(const std::string& filepath);
    Ref<Shader> Load(const std::string& name, const std::string& filepath);

    Ref<Shader> Get(const std::string& name);

    bool Exists(const std::string& name) const;

    void SetRootPath(const std::filesystem::path& path) {
        m_rootPath =
            path.is_relative() ? std::filesystem::current_path() / path : path;
    }

    std::filesystem::path GetAbsolutePath(
        const std::filesystem::path& filePath) {
        return filePath.is_relative() ? m_rootPath / filePath : filePath;
    }

   private:
    std::unordered_map<std::string, Ref<Shader>> m_shaders;
    std::filesystem::path m_rootPath;
};

}  // namespace SD

#endif /* SD_SHADER_HPP */
