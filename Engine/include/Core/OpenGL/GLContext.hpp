#ifndef SD_GL_CONTEXT_HPP
#define SD_GL_CONTEXT_HPP

#include "Utility/Export.hpp"
#include "Core/Context.hpp"

namespace sd {

class SD_API GLContext : public Context {
   public:
    GLContext(const WindowProp &property, SDL_Window **window);

    ~GLContext();

    void *getHandle() const override;

    void swapBuffer() const override;

   private:
    SDL_GLContext m_context;
    SDL_Window *m_window;
};

}  // namespace sd

#endif /* SD_GL_CONTEXT_HPP */
