#ifndef SD_SHADER_LOADER_HPP
#define SD_SHADER_LOADER_HPP

#include "Resource/Export.hpp"
#include "Graphics/Shader.hpp"

namespace SD {

class SD_RESOURCE_API ShaderLoader {
   public:
    Ref<Shader> Load(const std::string &comp_path);
    Ref<Shader> Load(const std::string &vert_path,
                     const std::string &frag_path);
    Ref<Shader> Load(const std::string &vert_path, const std::string &frag_path,
                     const std::string &geo_path);
};

}  // namespace SD
#endif  // !SD_SHADER_LOADER_HPP
