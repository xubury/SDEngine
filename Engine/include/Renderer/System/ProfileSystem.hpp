#ifndef SD_PROFILE_SYSTEM_HPP
#define SD_PROFILE_SYSTEM_HPP

#include "Core/System.hpp"
#include "Renderer/Font.hpp"
#include "Renderer/Camera.hpp"
#include "Utility/Timing.hpp"

namespace SD {

class ProfileSystem : public System {
   public:
    ProfileSystem(RenderTarget *target, int width, int height);

    void OnPush() override;
    void OnPop() override;
    void OnTick(float dt) override;
    void OnRender() override;
    void OnSizeEvent(const WindowSizeEvent &event);

   private:
    RenderTarget *m_target;
    OrthographicCamera m_camera;
    FpsCounter m_fps;
    Ref<Font> m_font;
};

}  // namespace SD

#endif /* SD_PROFILE_SYSTEM_HPP */
