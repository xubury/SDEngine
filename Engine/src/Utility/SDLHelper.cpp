#include "Utility/SDLHelper.hpp"
#include "Graphics/Texture.hpp"
#include <SDL_image.h>

namespace sd {

void flipBitmap(void *data, int width, int height) {
    char *pixels = static_cast<char *>(data);
    char *tmp = new char[width];

    for (int i = 0; i < height / 2; ++i) {
        char *row1 = pixels + i * width;
        char *row2 = pixels + (height - 1 - i) * width;
        memcpy(tmp, row1, width);
        memcpy(row1, row2, width);
        memcpy(row2, tmp, width);
    }

    delete[] tmp;
}

Ref<Texture> surfaceToTexture(SDL_Surface *surface) {
    if (Graphics::getAPI() == API::OpenGL) {
        SDL_LockSurface(surface);
        flipBitmap(surface->pixels, surface->pitch, surface->h);
        SDL_UnlockSurface(surface);
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
