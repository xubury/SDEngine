#ifndef SD_GL_WINDOW_HPP
#define SD_GL_WINDOW_HPP

#include "Utility/Base.hpp"
#include "Core/Window.hpp"

namespace SD {

class SD_CORE_API GLWindow : public Window {
   public:
    GLWindow(const WindowProp &property);

    ~GLWindow();

    void *GetGraphicsContext() override;

    uint8_t GetMSAA() const override;

    bool GetIsVSync() const override;

   private:
    void *m_context;
};

}  // namespace SD

#endif /* SD_GL_WINDOW_HPP */
