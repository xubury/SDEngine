#ifndef SD_GL_CONTEXT_HPP
#define SD_GL_CONTEXT_HPP

#include "Utils/Export.hpp"
#include <SDL.h>

namespace sd {

class Window;

class SD_API GLContext {
   public:
    bool create(SDL_Window *window);

    ~GLContext();

   private:
    SDL_GLContext m_context;
};

}  // namespace sd

#endif /* SD_GL_CONTEXT_HPP */
