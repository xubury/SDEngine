#include "Asset/ImageLoader.hpp"
#include <SDL_image.h>

namespace SD {

Ref<void> ImageLoader::LoadAsset(const std::string &path) {
    Ref<Image> image = CreateRef<Image>(path);
    SD_CORE_TRACE("Loading image form: {}...", path);
    if (!image->valid()) {
        SD_CORE_ERROR("Unable to load image {}! SDL_image Error: {}", path,
                      IMG_GetError());
    }
    return image;
}

}  // namespace SD
