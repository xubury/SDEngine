#ifndef SD_WINDOW_HPP
#define SD_WINDOW_HPP

#include <string>
#include "Core/SDL.hpp"
#include "Utility/Export.hpp"
#include "Utility/Base.hpp"

namespace sd {

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
    static void init(const WindowProp &property);

    virtual ~Window() = default;

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    static void swapBuffer();
    static bool pollEvent(SDL_Event &event);

    static bool shouldClose();
    static void setShouldClose(bool shouldClose);

    static int getWidth();

    static int getHeight();

    static SDL_Window *getHandle();

    static void *getGraphicsContext();

   protected:
    Window() = default;

    SDL_Window *m_window;
    void *m_context;
    bool m_shouldClose;
};

}  // namespace sd
#endif /* SD_WINDOW_HPP */
