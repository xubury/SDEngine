#ifndef SD_WINDOW_HPP
#define SD_WINDOW_HPP

#include <SDL2/SDL.h>
#include "Core/Export.hpp"

namespace sd {

class SD_API Window {
   public:
    Window();
    ~Window();

    bool init();
    void free();

    bool pollEvent(SDL_Event &event);

    bool shouldClose() const;
    void setShouldClose(bool shouldClose);

   private:
    SDL_Window *m_window;
    bool m_shouldClose;
};

}  // namespace sd

#endif /* WINDOW_HPP */
