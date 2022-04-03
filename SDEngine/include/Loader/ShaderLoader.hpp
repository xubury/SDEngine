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
// class ShaderAsset : public Asset {
//    public:
//     ShaderAsset(){};

//     void *LoadFromFile(const std::string &path) override {
//         *this = Asset::LoadArchiveFromFile<ShaderAsset>(path);
//         m_shader = ShaderLoader::LoadShader(m_shader_path);
//         return this;
//     }

//     Shader *GetShader() { return m_shader.get(); }

//     SERIALIZE(m_shader_path)

//    private:
//     Ref<Shader> m_shader;
//     std::string m_shader_path;
// };

}  // namespace SD
#endif  // !SD_SHADER_LOADER_HPP
