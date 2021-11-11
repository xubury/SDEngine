#ifndef SD_GL_WINDOW_HPP
#define SD_GL_WINDOW_HPP

#include "Utility/Export.hpp"
#include "Core/Window.hpp"

namespace SD {

class SD_API GLWindow : public Window {
   public:
    GLWindow(const WindowProp &property);

    ~GLWindow();

    void *getGraphicsContext() override;

   private:
    SDL_GLContext m_context;
};

}  // namespace SD

#endif /* SD_GL_WINDOW_HPP */
