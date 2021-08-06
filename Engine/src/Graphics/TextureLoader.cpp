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

// static void flipSurface(SDL_Surface *surface) {
//     SDL_LockSurface(surface);

//     const uint32_t pitch = surface->pitch;
//     char *tmp = new char[pitch];
//     char *pixels = static_cast<char *>(surface->pixels);

//     for (int i = 0; i < surface->h / 2; ++i) {
//         char *row1 = pixels + i * pitch;
//         char *row2 = pixels + (surface->h - 1 - i) * pitch;
//         memcpy(tmp, row1, pitch);
//         memcpy(row1, row2, pitch);
//         memcpy(row2, tmp, pitch);
//     }

//     delete[] tmp;
//     SDL_UnlockSurface(surface);
// }

TextureLoader::TextureLoader(AssetManager &manager)
    : AssetLoader<Texture>(manager) {}

Ref<Texture> TextureLoader::loadAsset(const std::string &path) {
    Ref<Texture> texture;
    SDL_Surface *image = loadSuface(m_manager.getRootPath() + path);
    // flipSurface(image);
    // TODO: parse SDL_Surface here
    TextureFormat format = TextureFormat::RGB;
    TextureFormatType formatType = TextureFormatType::UBYTE;
    switch (SDL_PIXELORDER(image->format->format)) {
        case SDL_ARRAYORDER_RGB:
            format = TextureFormat::RGB;
            formatType = TextureFormatType::UBYTE;
            break;
        case SDL_ARRAYORDER_RGBA:
            format = TextureFormat::RGBA;
            formatType = TextureFormatType::UBYTE;
            break;
        default:
            SD_CORE_WARN("Unsupported image type from: {} type: {}", path,
                         image->format->format);
    }
    texture = sd::Texture::create(
        image->w, image->h, sd::TextureType::TEX_2D, format, formatType,
        sd::TextureWrap::BORDER, sd::TextureFilter::NEAREST,
        sd::TextureMipmapFilter::NEAREST_LINEAR, image->pixels);

    SDL_FreeSurface(image);
    return texture;
}

}  // namespace sd
