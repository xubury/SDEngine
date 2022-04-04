#ifndef SD_WINDOW_HPP
#define SD_WINDOW_HPP

#include "Utility/Base.hpp"
#include "Utility/Math.hpp"
#include "Graphics/Graphics.hpp"
#include "Core/Export.hpp"
#include "Core/EventStack.hpp"
#include "Core/Layer.hpp"

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

    virtual void PollEvents(EventStack<Layer *> &layers) = 0;

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

    bool ShouldClose();
    void SetShouldClose(bool shouldClose);

   private:
    bool m_should_close;
};

}  // namespace SD
#endif /* SD_WINDOW_HPP */
