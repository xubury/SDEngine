#include "Utility/SDLHelper.hpp"
#include "Graphics/Texture.hpp"
#include <SDL_image.h>

namespace sd {

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

Ref<Texture> surfaceToTexture(SDL_Surface *surface) {
    if (Graphics::getAPI() == API::OpenGL) {
        flipSurface(surface);
    }
    // Convert other arrry order to RGB/RGBA, because it seems that OpenGL
    // can't handle certain array order.
    TextureFormat format;
    TextureFormatType formatType = TextureFormatType::UBYTE;
    SDL_Surface *converted;
    if (SDL_ISPIXELFORMAT_ALPHA(surface->format->format)) {
        SDL_PixelFormat *SDL_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
        converted = SDL_ConvertSurface(surface, SDL_format, 0);
        SDL_FreeFormat(SDL_format);
        format = TextureFormat::RGBA;
    } else {
        SDL_PixelFormat *SDL_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGB24);
        converted = SDL_ConvertSurface(surface, SDL_format, 0);
        SDL_FreeFormat(SDL_format);
        format = TextureFormat::RGB;
    }
    Ref<Texture> texture = Texture::create(
        converted->w, converted->h, 1, TextureType::TEX_2D, format, formatType,
        TextureWrap::REPEAT, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR_NEAREST, converted->pixels);

    SDL_FreeSurface(surface);
    SDL_FreeSurface(converted);
    return texture;
}

}  // namespace sd
