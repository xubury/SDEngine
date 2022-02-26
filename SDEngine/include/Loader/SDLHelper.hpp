#ifndef SD_SDL_HELPER_HPP
#define SD_SDL_HELPER_HPP

#include "Graphics/Graphics.hpp"

#include <SDL_image.h>
#include <string>

namespace SD {

SDL_Surface *LoadImage(const std::string &path);

int GetBitsPerPixel(const SDL_Surface *surface);

int GetNumOfChannels(const SDL_Surface *surface);

DataFormat GetDataFormat(const SDL_Surface *surface);

DataFormatType GetDataFormatType(const SDL_Surface *surface);

}  // namespace SD

#endif
