#ifndef SD_WINDOW_HPP
#define SD_WINDOW_HPP

#include <SDL.h>
#include <string>
#include "Core/Export.hpp"
#include "Core/GLContext.hpp"

namespace sd {

class SD_API Window {
   public:
    enum WindowFlag {
        WINDOWED = 0,
        INVISIBLE = 0x01,
        FULLSCREEN = 0x01 << 1,
        BORDERLESS = 0x01 << 2
    };

   public:
    Window();
    ~Window();

    bool create(const std::string &title, int width, int height,
                WindowFlag flags);
    void destroy();

    bool pollEvent(SDL_Event &event);

    bool shouldClose() const;
    void setShouldClose(bool shouldClose);

    void swapBuffer();

   private:
    GLContext m_context;
    SDL_Window *m_window;
    bool m_shouldClose;
};

}  // namespace sd

#endif /* WINDOW_HPP */
