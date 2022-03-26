#ifndef SD_WINDOW_HPP
#define SD_WINDOW_HPP

#include "Utility/Base.hpp"
#include "Utility/Math.hpp"
#include "Core/Export.hpp"
#include "Core/Event.hpp"
#include "Core/EventDispatcher.hpp"

#include <string>

namespace SD {

struct SD_CORE_API WindowCreateInfo {
    std::string title;
    int width;
    int height;
    MultiSampleLevel msaa;
    bool vsync;
};

class SD_CORE_API Window {
   public:
    static Scope<Window> Create(const WindowCreateInfo &info);

    Window();
    virtual ~Window() = default;

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    virtual Vector2i GetSize() const = 0;
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual MultiSampleLevel GetMSAA() const = 0;
    virtual bool GetIsVSync() const = 0;

    virtual std::string GetTitle() const = 0;

    virtual void ImGuiInitBackend() = 0;
    virtual void ImGuiShutdown() = 0;
    virtual void ImGuiNewFrame() = 0;
    virtual void ImGuiRenderDrawData() = 0;

    virtual void SwapBuffer() = 0;

    virtual void PollEvents() = 0;

    bool ShouldClose();
    void SetShouldClose(bool shouldClose);

    void SetDispatcher(EventDispatcher *dispatcher) {
        m_dispatcher = dispatcher;
    }
    EventDispatcher &GetDispatcher() { return *m_dispatcher; }

   private:
    bool m_should_close;
    EventDispatcher *m_dispatcher;
};

}  // namespace SD
#endif /* SD_WINDOW_HPP */
