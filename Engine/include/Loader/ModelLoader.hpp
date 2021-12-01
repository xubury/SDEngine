#ifndef SD_MODEL_LOADER_HPP
#define SD_MODEL_LOADER_HPP

#include "Loader/Export.hpp"
#include "Asset/Asset.hpp"

namespace SD {

class SD_LOADER_API ModelLoader : public AssetLoaderBase {
   public:
    using AssetLoaderBase::AssetLoaderBase;

    Ref<void> LoadAsset(const std::string &path) override;
};

}  // namespace SD

#endif /* SD_MODEL_LOADER_HPP */
