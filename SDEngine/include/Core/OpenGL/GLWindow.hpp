#ifndef SD_GL_WINDOW_HPP
#define SD_GL_WINDOW_HPP

#include "Utility/Base.hpp"
#include "Core/Window.hpp"

class SDL_Window;

namespace SD {

class SD_CORE_API GLWindow : public Window {
   public:
    GLWindow(const WindowCreateInfo &info);
    ~GLWindow();
    void PollEvents() override;

    Vector2i GetSize() const override;
    int GetWidth() const override;
    int GetHeight() const override;
    MultiSampleLevel GetMSAA() const override;

    bool GetIsVSync() const override;

    std::string GetTitle() const override;

    void ImGuiInitBackend() override;
    void ImGuiShutdown() override;
    void ImGuiNewFrame() override;
    void ImGuiRenderDrawData() override;

   private:
    void SwapBuffer() override;

    void *m_context;
    SDL_Window *m_window;
    bool m_is_init_imgui;
};

}  // namespace SD

#endif /* SD_GL_WINDOW_HPP */
