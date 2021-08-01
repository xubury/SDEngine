#include "Graphics/TextureLoader.hpp"
#include "Utils/AssetManager.hpp"
#include <SDL_image.h>

namespace sd {

static SDL_Surface *loadSuface(const std::string &path) {
    // Load image at specified path
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        SD_CORE_ERROR("Unable to load image {}! SDL_image Error: {}",
                      path.c_str(), IMG_GetError());
    }
    return loadedSurface;
}

TextureLoader::TextureLoader(AssetManager &manager)
    : AssetLoader<Texture>(manager) {}

Ref<Texture> TextureLoader::loadAsset(const std::string &path) {
    Ref<Texture> texture;
    SDL_Surface *image = loadSuface(m_manager.getRootPath() + path);
    // TODO: parse SDL_Surface here
    texture = sd::Texture::create(
        image->w, image->h, sd::TextureType::TEX_2D, sd::TextureFormat::RGB,
        sd::TextureFormatType::UBYTE, sd::TextureWrap::BORDER,
        sd::TextureFilter::NEAREST, sd::TextureMipmapFilter::NEAREST_LINEAR,
        image->pixels);

    SDL_FreeSurface(image);
    return texture;
}

}  // namespace sd
