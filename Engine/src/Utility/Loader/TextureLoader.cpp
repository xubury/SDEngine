#include "Utility/Loader/TextureLoader.hpp"
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
    : AssetLoader<Texture>(manager), m_flip(false) {
    int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
        SD_CORE_ERROR("IMG_Init Failed: {}", IMG_GetError());
        exit(-1);
    }
}

TextureLoader::~TextureLoader() { IMG_Quit(); }

void TextureLoader::setFlipVertically(bool flip) { m_flip = flip; }

Ref<Texture> TextureLoader::loadAsset(const std::string &filePath) {
    Ref<Texture> texture;
    SD_CORE_TRACE("Loading texture form: {}...", filePath);

    SDL_Surface *image = loadSuface(filePath);
    if (m_flip) {
        flipSurface(image);
    }
    // Convert other arrry order to RGB/RGBA, because it seems that OpenGL can't
    // handle certain array order.
    TextureFormat format;
    TextureFormatType formatType = TextureFormatType::UBYTE;
    SDL_Surface *converted;
    if (SDL_ISPIXELFORMAT_ALPHA(image->format->format)) {
        SDL_PixelFormat *SDL_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
        converted = SDL_ConvertSurface(image, SDL_format, 0);
        SDL_FreeFormat(SDL_format);
        format = TextureFormat::RGBA;
    } else {
        SDL_PixelFormat *SDL_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGB24);
        converted = SDL_ConvertSurface(image, SDL_format, 0);
        SDL_FreeFormat(SDL_format);
        format = TextureFormat::RGB;
    }
    texture = Texture::create(
        converted->w, converted->h, 1, TextureType::TEX_2D, format, formatType,
        TextureWrap::REPEAT, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR_NEAREST, converted->pixels);

    SDL_FreeSurface(image);
    SDL_FreeSurface(converted);
    return texture;
}

}  // namespace sd
