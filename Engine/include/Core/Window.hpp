#ifndef SD_WINDOW_HPP
#define SD_WINDOW_HPP

#include "Utility/Base.hpp"
#include "Core/Export.hpp"
#include "Graphics/Context.hpp"

#include <glm/glm.hpp>
#include <string>

union SDL_Event;
struct SDL_Window;

namespace SD {

struct SD_CORE_API WindowProp {
    std::string title;
    int x;
    int y;
    int width;
    int height;
    int msaa;
    bool vsync;
    uint32_t flag;
    WindowProp();
};

class SD_CORE_API Window : public Context {
   public:
    static Ref<Window> Create(const WindowProp &property);

    virtual ~Window();

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    glm::ivec2 GetSize() const override;
    int GetWidth() const override;
    int GetHeight() const override;

    SDL_Window *GetHandle();

    virtual void *GetGraphicsContext() = 0;

    virtual bool GetIsVSync() const = 0;

    std::string GetTitle() const;

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
