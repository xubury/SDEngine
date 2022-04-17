#ifndef SD_SHADER_MANAGER_HPP
#define SD_SHADER_MANAGER_HPP

#include "Resource/ResourceCache.hpp"
#include "Resource/ShaderLoader.hpp"

namespace SD {

struct ShaderLoaderAdaptor : ShaderLoader {
    template <typename... Args>
    Ref<Shader> operator()(Args &&...args)
    {
        return ShaderLoader::Load(std::forward<Args>(args)...);
    }
};

using ShaderCache = ResourceCache<Shader, ShaderLoaderAdaptor>;
using ShaderHandle = ResourceHandle<Shader>;
class ShaderManager {
   public:
    ShaderHandle LoadShader(const std::string_view &identifier,
                            const std::string_view &vert_path,
                            const std::string_view &frag_path)
    {
        return m_shaders.Load(ResourceId(identifier), m_directory / vert_path,
                              m_directory / frag_path);
    }

    ShaderHandle LoadShader(const std::string_view &identifier,
                            const std::string_view &vert_path,
                            const std::string_view &frag_path,
                            const std::string_view &geom_path)
    {
        return m_shaders.Load(ResourceId(identifier), m_directory / vert_path,
                              m_directory / frag_path, m_directory / geom_path);
    }

    ShaderHandle LoadComputeShader(const std::string_view &identifier,
                                   const std::string_view &path)

    {
        return m_shaders.Load(ResourceId(identifier), m_directory / path);
    }

    static ShaderManager &Get();

    void Init(std::filesystem::path directory);

   private:
    ShaderManager() = default;
    ~ShaderManager() = default;

    ShaderCache m_shaders;
    std::filesystem::path m_directory;
};

}  // namespace SD

#endif
