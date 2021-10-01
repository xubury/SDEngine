#include "Utility/Loader/TextureLoader.hpp"
#include "Utility/SDLHelper.hpp"
#include <SDL_image.h>

namespace sd {

TextureLoader::TextureLoader(AssetManager &manager)
    : AssetLoader<Texture>(manager) {
    int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
        SD_CORE_ERROR("IMG_Init Failed: {}", IMG_GetError());
        exit(-1);
    }
}

TextureLoader::~TextureLoader() { IMG_Quit(); }

Ref<Texture> TextureLoader::loadAsset(const std::string &filePath) {
    Ref<Texture> texture;
    SD_CORE_TRACE("Loading texture form: {}...", filePath);

    SDL_Surface *image = IMG_Load(filePath.c_str());
    if (image == nullptr) {
        SD_CORE_ERROR("Unable to load image {}! SDL_image Error: {}",
                      filePath.c_str(), IMG_GetError());
    } else {
        texture = surfaceToTexture(image);
    }
    return texture;
}

}  // namespace sd
