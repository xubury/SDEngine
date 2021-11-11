#ifndef SD_WINDOW_HPP
#define SD_WINDOW_HPP

#include <string>
#include <SDL.h>
#include "Utility/Export.hpp"
#include "Utility/Base.hpp"

namespace SD {

#ifdef DEBUG_BUILD
#define SDL(stmt) SD_CORE_ASSERT(stmt == 0, SDL_GetError())
#else
#define SDL(stmt) stmt
#endif

struct WindowProp {
    std::string title;
    int x;
    int y;
    int width;
    int height;
    int samples;
    bool vsync;
    uint32_t flag;
    WindowProp()
        : title("SD Engine"),
          x(SDL_WINDOWPOS_CENTERED),
          y(SDL_WINDOWPOS_CENTERED),
          width(1440),
          height(900),
          samples(8),
          vsync(true),
          flag(0) {}
};

class SD_API Window {
   public:
    static Ref<Window> create(const WindowProp &property);

    virtual ~Window();

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    void swapBuffer();
    bool pollEvent(SDL_Event &event);

    bool shouldClose();
    void setShouldClose(bool shouldClose);

    int getWidth();

    int getHeight();

    SDL_Window *getHandle();

    virtual void *getGraphicsContext() = 0;

   protected:
    Window() = default;

    SDL_Window *m_window;
    bool m_shouldClose;
};

}  // namespace SD
#endif /* SD_WINDOW_HPP */
