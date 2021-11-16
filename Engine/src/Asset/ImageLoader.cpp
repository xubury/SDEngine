#include "Asset/ImageLoader.hpp"
#include <SDL_image.h>

namespace SD {

Ref<void> ImageLoader::loadAsset(const std::string &filePath) {
    Ref<Image> image = createRef<Image>(filePath);
    SD_CORE_TRACE("Loading image form: {}...", filePath);
    if (!image->valid()) {
        SD_CORE_ERROR("Unable to load image {}! SDL_image Error: {}",
                      filePath.c_str(), IMG_GetError());
    }
    return image;
}

}  // namespace SD
