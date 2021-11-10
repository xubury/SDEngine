#include "Asset/Image.hpp"
#include <SDL_image.h>

namespace sd {

Image::Image(const std::string &filePath) : m_surface(nullptr) {
    SDL_Surface *loaded = IMG_Load(filePath.c_str());
    if (loaded) {
        if (SDL_ISPIXELFORMAT_ALPHA(loaded->format->format)) {
            SDL_PixelFormat *SDL_format =
                SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
            m_surface = SDL_ConvertSurface(loaded, SDL_format, 0);
            SDL_FreeFormat(SDL_format);
        } else {
            SDL_PixelFormat *SDL_format =
                SDL_AllocFormat(SDL_PIXELFORMAT_RGB24);
            m_surface = SDL_ConvertSurface(loaded, SDL_format, 0);
            SDL_FreeFormat(SDL_format);
        }
        SDL_FreeSurface(loaded);
    }
}

Image::~Image() { SDL_FreeSurface(m_surface); }

bool Image::valid() const { return m_surface != nullptr; }

const void *Image::data() const { return m_surface->pixels; }

void *Image::data() { return m_surface->pixels; }

uint32_t Image::width() const { return m_surface->w; }

uint32_t Image::height() const { return m_surface->h; }

bool Image::hasAlpha() const {
    return SDL_ISPIXELFORMAT_ALPHA(m_surface->format->format);
}

}  // namespace sd
