#include "Loader/SDLHelper.hpp"
#include "Utility/Base.hpp"

namespace SD {

SDL_Surface* LoadImage(const std::string& path) {
    SDL_Surface* loaded = IMG_Load(path.c_str());
    SD_CORE_TRACE("Loading image form: {}...", path);
    if (loaded == nullptr) {
        throw Exception(
            fmt::format("Unable to load image {}! SDL_image Error: {}", path,
                        IMG_GetError()));
    }
    return loaded;
}

int GetBitsPerPixel(SDL_Surface* surface) {
    return surface->format->BitsPerPixel;
}

int GetNumOfChannels(SDL_Surface* surface) {
    return surface->pitch / surface->w;
}

}  // namespace SD
