#ifndef SD_SHADER_LOADER_HPP
#define SD_SHADER_LOADER_HPP

#include "Utils/AssetLoader.hpp"
#include "Graphics/Shader.hpp"

namespace sd {

class SD_API ShaderLoader : public AssetLoader<Shader> {
   public:
    ShaderLoader(AssetManager &manager);

    Ref<Shader> loadAsset(const std::string &filePath) override;
};

}  // namespace sd

#endif /* SD_SHADER_LOADER_HPP */
