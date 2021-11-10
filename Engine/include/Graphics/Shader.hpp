#ifndef SD_SHADER_HPP
#define SD_SHADER_HPP

#include "Utility/Base.hpp"
#include "Utility/Export.hpp"
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <unordered_map>
#include <filesystem>

namespace sd {

class Texture;
class UniformBuffer;

enum class ShaderType { INVALID, VERTEX, FRAGMENT, GEOMETRY, COMPUTE };

class SD_API Shader {
   public:
    static Ref<Shader> create(const std::string& filePath);

    virtual ~Shader() = default;

    Shader(const Shader&) = delete;

    Shader& operator=(const Shader&) = delete;

    virtual void compileShader(ShaderType type, const char* code) = 0;

    virtual void linkShaders() = 0;

    virtual void bind() = 0;

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
    virtual void setTexture(const uint32_t id, const Texture* texture) = 0;

    virtual void setUniformBuffer(const std::string& name,
                                  const UniformBuffer& buffer) = 0;

   protected:
    Shader() = default;
};

class ShaderLibrary {
   public:
    static ShaderLibrary& instance() {
        static ShaderLibrary s_instance;
        return s_instance;
    }
    void add(const std::string& name, const Ref<Shader>& shader);
    Ref<Shader> load(const std::string& filepath);
    Ref<Shader> load(const std::string& name, const std::string& filepath);

    Ref<Shader> get(const std::string& name);

    bool exists(const std::string& name) const;

    void setRootPath(const std::filesystem::path& path) {
        m_rootPath =
            path.is_relative() ? std::filesystem::current_path() / path : path;
    }

    std::filesystem::path getAbsolutePath(
        const std::filesystem::path &filePath) {
        return filePath.is_relative() ? m_rootPath / filePath : filePath;
    }

   private:
    std::unordered_map<std::string, Ref<Shader>> m_Shaders;
    std::filesystem::path m_rootPath;
};

}  // namespace sd

#endif /* SD_SHADER_HPP */
