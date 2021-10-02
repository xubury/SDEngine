#ifndef SD_SDL_HELPER_HPP
#define SD_SDL_HELPER_HPP

#include "Utility/Base.hpp"
#include "Utility/Export.hpp"

class SDL_Surface;

namespace sd {

class Texture;

void flipBitmap(void *data, int width, int height);

Ref<Texture> surfaceToTexture(SDL_Surface *surface);

}  // namespace sd

#endif /* SD_SDL_HELPER_HPP */
