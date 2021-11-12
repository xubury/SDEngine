#include "Asset/ImageLoader.hpp"
#include <SDL_image.h>

namespace SD {

ImageLoader::ImageLoader() {
    int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
        SD_CORE_ERROR("IMG_Init Failed: {}", IMG_GetError());
        exit(-1);
    }
}

ImageLoader::~ImageLoader() { IMG_Quit(); }

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
