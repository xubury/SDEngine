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
    SDL_WindowFlags flag;
    WindowProp()
        : title("SD Engine"),
          x(SDL_WINDOWPOS_CENTERED),
          y(SDL_WINDOWPOS_CENTERED),
          width(1440),
          height(900),
          samples(8),
          vsync(true),
          flag(SDL_WindowFlags(0)) {}
};

class SD_API Window {
   public:
    static Ref<Window> create(const WindowProp &property);

    virtual ~Window() = default;

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    void swapBuffer() const;
    bool pollEvent(SDL_Event &event);

    bool shouldClose() const;
    void setShouldClose(bool shouldClose);

    int getWidth() const;

    int getHeight() const;

    SDL_Window *getHandle() const;

    virtual void *getGraphicsContext() const = 0;

   protected:
    Window() = default;

    SDL_Window *m_window;
    bool m_shouldClose;
};

}  // namespace sd
#endif /* SD_WINDOW_HPP */
