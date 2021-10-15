#ifndef SD_SDL_HPP
#define SD_SDL_HPP

#include <SDL.h>

#ifdef DEBUG_BUILD
#define SDL(stmt) \
    if (stmt != 0) SD_CORE_ERROR(#stmt " Failed: {}", SDL_GetError())
#else
#define SDL(stmt) stmt
#endif

#endif  // SD_SDL_HPP
