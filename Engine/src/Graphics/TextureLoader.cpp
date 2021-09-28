#include "Graphics/TextureLoader.hpp"
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

static void flipSurface(SDL_Surface *surface) {
    SDL_LockSurface(surface);

    const uint32_t pitch = surface->pitch;
    char *tmp = new char[pitch];
    char *pixels = static_cast<char *>(surface->pixels);

    for (int i = 0; i < surface->h / 2; ++i) {
        char *row1 = pixels + i * pitch;
        char *row2 = pixels + (surface->h - 1 - i) * pitch;
        memcpy(tmp, row1, pitch);
        memcpy(row1, row2, pitch);
        memcpy(row2, tmp, pitch);
    }

    delete[] tmp;
    SDL_UnlockSurface(surface);
}

TextureLoader::TextureLoader(AssetManager &manager)
    : AssetLoader<Texture>(manager), m_flip(false) {}

void TextureLoader::setFlipVertically(bool flip) { m_flip = flip; }

Ref<Texture> TextureLoader::loadAsset(const std::string &filePath) {
    Ref<Texture> texture;
    SD_CORE_TRACE("Loading texture form: {}...", filePath);

    SDL_Surface *image = loadSuface(filePath);
    if (m_flip) {
        flipSurface(image);
    }
    // TODO: parse SDL_Surface here
    TextureFormat format = TextureFormat::RGB;
    TextureFormatType formatType = TextureFormatType::UBYTE;
    if (SDL_ISPIXELFORMAT_ALPHA(image->format->format)) {
        format = TextureFormat::RGBA;
    }
    texture = sd::Texture::create(
        image->w, image->h, 1, sd::TextureType::TEX_2D, format, formatType,
        sd::TextureWrap::REPEAT, sd::TextureFilter::LINEAR,
        sd::TextureMipmapFilter::LINEAR_NEAREST, image->pixels);

    SDL_FreeSurface(image);
    return texture;
}

}  // namespace sd
