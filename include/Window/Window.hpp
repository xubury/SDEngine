#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <SDL2/SDL.h>

namespace S2D {

class Window {
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

}  // namespace S2D

#endif /* WINDOW_HPP */
