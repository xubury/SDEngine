#ifndef SD_SDL_HPP
#define SD_SDL_HPP

#include <SDL.h>

#ifdef DEBUG_BUILD
#define SDL(stmt) SD_CORE_ASSERT(stmt == 0, SDL_GetError())
#else
#define SDL(stmt) stmt
#endif

#endif  // SD_SDL_HPP
