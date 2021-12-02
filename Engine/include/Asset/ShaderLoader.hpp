#ifndef SD_SHADER_LOADER_HPP
#define SD_SHADER_LOADER_HPP

#include "Asset/Export.hpp"
#include "Asset/Asset.hpp"

namespace SD {

class SD_ASSET_API ShaderLoader : public AssetLoader {
   public:
    using AssetLoader::AssetLoader;
    Ref<void> LoadAsset(const std::string &path) override;
};

}  // namespace SD

#endif  // !SD_SHADER_LOADER_HPP
