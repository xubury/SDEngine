#ifndef SD_IMAGE_LOADER_HPP
#define SD_IMAGE_LOADER_HPP

#include "Asset/Asset.hpp"
#include "Asset/Image.hpp"

namespace SD {

class SD_API ImageLoader : public AssetLoaderBase {
   private:
    friend class AssetManager;

    ImageLoader();

   public:
    Ref<void> loadAsset(const std::string &filePath) override;

    ~ImageLoader();
};

}  // namespace SD

#endif
