#ifndef SD_WINDOW_HPP
#define SD_WINDOW_HPP

#include "Utility/Base.hpp"

#include <glm/glm.hpp>
#include <string>

union SDL_Event;
struct SDL_Window;

namespace SD {

struct SD_API WindowProp {
    std::string title;
    int x;
    int y;
    int width;
    int height;
    int msaa;
    bool vsync;
    uint32_t flag;
    WindowProp()
        : title("SD Engine"),
          x(0),
          y(0),
          width(1440),
          height(900),
          msaa(8),
          vsync(true),
          flag(0) {}
    WindowProp(const std::string &title, int x, int y, int width, int height,
               int msaa, bool vsync, uint32_t flag)
        : title(title),
          x(x),
          y(y),
          width(width),
          height(height),
          msaa(msaa),
          vsync(vsync),
          flag(flag) {}
};

class RenderTarget;

class SD_API Window {
   public:
    static Ref<Window> Create(const WindowProp &property);

    virtual ~Window();

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    glm::ivec2 GetSize();

    SDL_Window *GetHandle();

    virtual void *GetGraphicsContext() = 0;

    virtual uint8_t GetMSAA() const = 0;

    virtual bool GetIsVSync() const = 0;

    std::string GetTitle() const;

    glm::vec2 MapScreenToClip(const RenderTarget &target,
                              const glm::ivec2 &pos) const;
    glm::ivec2 MapClipToScreen(const RenderTarget &target,
                               const glm::vec2 &pos) const;

   protected:
    Window() = default;

    SDL_Window *m_window;

   private:
    friend class Application;

    void SwapBuffer();
    bool PollEvent(SDL_Event &event);

    bool ShouldClose();
    void SetShouldClose(bool shouldClose);

    bool m_shouldClose;
};

}  // namespace SD
#endif /* SD_WINDOW_HPP */
