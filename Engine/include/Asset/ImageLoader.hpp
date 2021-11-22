#ifndef SD_IMAGE_LOADER_HPP
#define SD_IMAGE_LOADER_HPP

#include "Asset/Asset.hpp"
#include "Asset/Image.hpp"

namespace SD {

class SD_API ImageLoader : public AssetLoaderBase {
   public:
    using AssetLoaderBase::AssetLoaderBase;

    Ref<void> LoadAsset(const std::filesystem::path &path) override;
};

}  // namespace SD

#endif
