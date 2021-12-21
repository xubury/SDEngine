#ifndef SD_GL_WINDOW_HPP
#define SD_GL_WINDOW_HPP

#include "Utility/Base.hpp"
#include "Core/Window.hpp"

class SDL_Window;

namespace SD {

class SD_CORE_API GLWindow : public Window {
   public:
    GLWindow(const WindowProp &property);
    ~GLWindow();

    glm::ivec2 GetSize() const override;
    int GetWidth() const override;
    int GetHeight() const override;
    uint8_t GetMSAA() const override;

    void *GetHandle() const override;

    void *GetGraphicsContext() override;

    bool GetIsVSync() const override;

    std::string GetTitle() const override;

   private:
    void SwapBuffer() override;

    void *m_context;
    SDL_Window *m_window;
};

}  // namespace SD

#endif /* SD_GL_WINDOW_HPP */
