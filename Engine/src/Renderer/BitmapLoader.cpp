#include "Renderer/BitmapLoader.hpp"

#include <SDL_image.h>

namespace SD {

BitmapLoader::BitmapLoader(AssetManager &manager) : AssetLoader(manager) {
    int img_flags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF;
    SD_CORE_ASSERT((IMG_Init(img_flags) & img_flags) == img_flags,
                   IMG_GetError());
}

BitmapLoader::~BitmapLoader() { IMG_Quit(); }

Ref<void> BitmapLoader::LoadAsset(const std::string &path) {
    SDL_Surface *loaded = IMG_Load(path.c_str());
    SD_CORE_TRACE("Loading image form: {}...", path);
    Ref<Bitmap> image;
    if (loaded == nullptr) {
        SD_CORE_ERROR("Unable to load image {}! SDL_image Error: {}", path,
                      IMG_GetError());
    } else {
        SDL_Surface *surface = nullptr;
        uint8_t channels = 0;
        if (SDL_ISPIXELFORMAT_ALPHA(loaded->format->format)) {
            SDL_PixelFormat *SDL_format =
                SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
            surface = SDL_ConvertSurface(loaded, SDL_format, 0);
            SDL_FreeFormat(SDL_format);
            channels = 4;
        } else {
            SDL_PixelFormat *SDL_format =
                SDL_AllocFormat(SDL_PIXELFORMAT_RGB24);
            surface = SDL_ConvertSurface(loaded, SDL_format, 0);
            SDL_FreeFormat(SDL_format);
            channels = 3;
        }
        SD_CORE_ASSERT(surface && channels > 0, "SDL_ConvertSurface failed!");
        image = CreateRef<Bitmap>(surface->w, surface->h, channels,
                                  static_cast<uint8_t *>(surface->pixels));
        SDL_FreeSurface(loaded);
        SDL_FreeSurface(surface);
    }

    return image;
}

}  // namespace SD
