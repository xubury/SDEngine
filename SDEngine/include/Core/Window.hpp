#ifndef SD_WINDOW_HPP
#define SD_WINDOW_HPP

#include "Utility/Base.hpp"
#include "Core/Export.hpp"
#include "Core/Event/Event.hpp"

#include <glm/glm.hpp>
#include <string>

namespace SD {

struct SD_CORE_API WindowCreateInfo {
    std::string title;
    int x;
    int y;
    int width;
    int height;
    MultiSampleLevel msaa;
    bool vsync;
    uint32_t flag;
};

class SD_CORE_API Window {
   public:
    static Scope<Window> Create(const WindowCreateInfo &info);

    Window() = default;
    virtual ~Window() = default;

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    virtual glm::ivec2 GetSize() const = 0;
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

    void PollEvents();

    bool ShouldClose();
    void SetShouldClose(bool shouldClose);

   private:
    bool m_should_close;
};

}  // namespace SD
#endif /* SD_WINDOW_HPP */
