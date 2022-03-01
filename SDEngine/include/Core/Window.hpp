#ifndef SD_WINDOW_HPP
#define SD_WINDOW_HPP

#include "Utility/Base.hpp"
#include "Core/Export.hpp"
#include "Core/Event.hpp"
#include "Graphics/Context.hpp"

#include <glm/glm.hpp>
#include <string>

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
    WindowProp(const std::string &title, int32_t x, int32_t y, int32_t width,
               int32_t height, int32_t msaa, bool vsync, uint32_t flag);
};

class SD_CORE_API Window : public Context {
   public:
    static Ref<Window> Create(const WindowProp &property);

    virtual ~Window() = default;

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    virtual void *GetHandle() const = 0;

    virtual void *GetGraphicsContext() = 0;

    virtual bool GetIsVSync() const = 0;

    virtual std::string GetTitle() const = 0;

   protected:
    Window() = default;

   private:
    friend class Application;

    virtual void SwapBuffer() = 0;

    bool PollEvent(Event &event);

    bool ShouldClose();
    void SetShouldClose(bool shouldClose);

    bool m_should_close;
};

}  // namespace SD
#endif /* SD_WINDOW_HPP */
