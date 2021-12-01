#ifndef SD_IMAGE_LOADER_HPP
#define SD_IMAGE_LOADER_HPP

#include "Loader/Export.hpp"
#include "Asset/Asset.hpp"
#include "Asset/Image.hpp"

namespace SD {

class SD_LOADER_API ImageLoader : public AssetLoaderBase {
   public:
    using AssetLoaderBase::AssetLoaderBase;

    Ref<void> LoadAsset(const std::string &path) override;
};

}  // namespace SD

#endif
