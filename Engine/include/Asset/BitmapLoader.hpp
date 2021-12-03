#ifndef SD_IMAGE_LOADER_HPP
#define SD_IMAGE_LOADER_HPP

#include "Asset/Export.hpp"
#include "Asset/Asset.hpp"
#include "Asset/Bitmap.hpp"

namespace SD {

class SD_ASSET_API BitmapLoader : public AssetLoader {
   public:
    using AssetLoader::AssetLoader;

    Ref<void> LoadAsset(const std::string &path) override;
};

}  // namespace SD

#endif
