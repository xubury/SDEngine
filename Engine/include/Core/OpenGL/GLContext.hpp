#ifndef SD_GL_CONTEXT_HPP
#define SD_GL_CONTEXT_HPP

#include "Utils/Export.hpp"
#include "Core/Context.hpp"
#include <SDL.h>

namespace sd {

class Window;

class SD_API GLContext : public Context {
   public:
    GLContext(SDL_Window *window);

    ~GLContext();

    void *getHandle() const override;

   private:
    friend class Window;
    SDL_GLContext m_context;
};

}  // namespace sd

#endif /* SD_GL_CONTEXT_HPP */
