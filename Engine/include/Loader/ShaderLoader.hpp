#ifndef SD_SHADER_LOADER_HPP
#define SD_SHADER_LOADER_HPP

#include "Loader/Export.hpp"
#include "Asset/Asset.hpp"

namespace SD {

class SD_LOADER_API ShaderLoader : public AssetLoaderBase {
   public:
    using AssetLoaderBase::AssetLoaderBase;
    Ref<void> LoadAsset(const std::string &path) override;
};

}  // namespace SD

#endif  // !SD_SHADER_LOADER_HPP
