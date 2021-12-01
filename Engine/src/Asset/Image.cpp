#include "Asset/Image.hpp"

#include <SDL.h>

namespace SD {

Image::Image(SDL_Surface *surface) : m_surface(surface) {}

Image::~Image() { SDL_FreeSurface(m_surface); }

bool Image::Valid() const { return m_surface != nullptr; }

const void *Image::Data() const { return m_surface->pixels; }

void *Image::Data() { return m_surface->pixels; }

uint32_t Image::Width() const { return m_surface->w; }

uint32_t Image::Height() const { return m_surface->h; }

bool Image::HasAlpha() const {
    return SDL_ISPIXELFORMAT_ALPHA(m_surface->format->format);
}

}  // namespace SD
