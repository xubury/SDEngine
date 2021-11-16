#ifndef SD_WINDOW_HPP
#define SD_WINDOW_HPP

#include "Utility/Base.hpp"

#include <glm/glm.hpp>
#include <string>

union SDL_Event;
struct SDL_Window;

namespace SD {

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
          x(0),
          y(0),
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

    glm::ivec2 getSize();

    uint8_t getSamples() const { return m_samples; }

    SDL_Window *getHandle();

    virtual void *getGraphicsContext() = 0;

   protected:
    Window(const WindowProp &property);

    SDL_Window *m_window;

   private:
    bool m_shouldClose;
    uint8_t m_samples;
};

}  // namespace SD
#endif /* SD_WINDOW_HPP */
