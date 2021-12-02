#include "Asset/ImageLoader.hpp"

#include <SDL_image.h>

namespace SD {

Ref<void> ImageLoader::LoadAsset(const std::string &path) {
    SDL_Surface *loaded = IMG_Load(path.c_str());
    SD_CORE_TRACE("Loading image form: {}...", path);
    Ref<Image> image;
    if (loaded == nullptr) {
        SD_CORE_ERROR("Unable to load image {}! SDL_image Error: {}", path,
                      IMG_GetError());
    } else {
        SDL_Surface *surface = nullptr;
        if (SDL_ISPIXELFORMAT_ALPHA(loaded->format->format)) {
            SDL_PixelFormat *SDL_format =
                SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
            surface = SDL_ConvertSurface(loaded, SDL_format, 0);
            SDL_FreeFormat(SDL_format);
        } else {
            SDL_PixelFormat *SDL_format =
                SDL_AllocFormat(SDL_PIXELFORMAT_RGB24);
            surface = SDL_ConvertSurface(loaded, SDL_format, 0);
            SDL_FreeFormat(SDL_format);
        }
        SD_CORE_ASSERT(surface, "SDL_ConvertSurface failed!");
        image = CreateRef<Image>(surface);
        SDL_FreeSurface(loaded);
    }

    return image;
}

}  // namespace SD
