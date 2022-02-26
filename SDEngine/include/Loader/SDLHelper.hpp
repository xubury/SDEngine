#ifndef SD_SDL_HELPER_HPP
#define SD_SDL_HELPER_HPP

#include <SDL_image.h>
#include <string>

namespace SD {

SDL_Surface *LoadImage(const std::string &path);

int GetBitsPerPixel(SDL_Surface *surface);

int GetNumOfChannels(SDL_Surface *surface);

}  // namespace SD

#endif
