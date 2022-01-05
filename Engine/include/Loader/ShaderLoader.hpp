#ifndef SD_SHADER_LOADER_HPP
#define SD_SHADER_LOADER_HPP

#include "Loader/Export.hpp"
#include "Asset/AssetManager.hpp"

namespace SD {

class SD_LOADER_API ShaderLoader : public AssetLoader {
   public:
    using AssetLoader::AssetLoader;

    Ref<void> LoadAsset(const std::string &path) override;
};

}  // namespace SD
#endif  // !SD_SHADER_LOADER_HPP
