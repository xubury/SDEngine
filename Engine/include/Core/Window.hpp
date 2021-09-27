#ifndef SD_WINDOW_HPP
#define SD_WINDOW_HPP

#include <SDL.h>
#include <string>
#include "Utils/Export.hpp"
#include "Utils/Base.hpp"

namespace sd {

enum WindowFlag {
    WINDOWED = 0,
    INVISIBLE = 0x01,
    FULLSCREEN = 0x01 << 1,
    BORDERLESS = 0x01 << 2
};

struct WindowProp {
    std::string title;
    int x;
    int y;
    int width;
    int height;
    int samples;
    WindowFlag flag;
    WindowProp()
        : title("SD Engine"),
          x(SDL_WINDOWPOS_CENTERED),
          y(SDL_WINDOWPOS_CENTERED),
          width(1440),
          height(900),
          samples(8),
          flag(WINDOWED) {}
};

class Context;

class SD_API Window {
   public:
    Window();
    ~Window();
    Window(const Window &other) = delete;
    Window &operator=(const Window &other) = delete;

    bool create(const WindowProp &property);

    bool pollEvent(SDL_Event &event);

    bool shouldClose() const;
    void setShouldClose(bool shouldClose);

    void swapBuffer();

    int getWidth() const;

    int getHeight() const;

    SDL_Window *getHandle() const;

    void *getGraphicContext() const;

   private:
    Ref<Context> m_context;
    SDL_Window *m_window;
    bool m_shouldClose;
};

}  // namespace sd

#endif /* WINDOW_HPP */
