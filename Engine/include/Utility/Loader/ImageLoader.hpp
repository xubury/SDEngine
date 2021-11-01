#ifndef SD_IMAGE_LOADER_HPP
#define SD_IMAGE_LOADER_HPP

#include "Utility/Loader/AssetLoader.hpp"
#include "Utility/Image.hpp"

namespace sd {
class SD_API ImageLoader : public AssetLoader<Image> {
   public:
    ImageLoader(AssetManager &manager);
    ~ImageLoader();

    Ref<Image> loadAsset(const std::string &filePath) override;
};
}  // namespace sd

#endif
