#ifndef SD_SHADER_LOADER_HPP
#define SD_SHADER_LOADER_HPP

#include "Loader/Export.hpp"
#include "Graphics/Shader.hpp"

namespace SD {

class SD_LOADER_API ShaderLoader {
   public:
    static Ref<Shader> LoadShader(const std::string &comp_path);
    static Ref<Shader> LoadShader(const std::string &vert_path,
                                  const std::string &frag_path);
    static Ref<Shader> LoadShader(const std::string &vert_path,
                                  const std::string &frag_path,
                                  const std::string &geo_path);
};

}  // namespace SD
#endif  // !SD_SHADER_LOADER_HPP
