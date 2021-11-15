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

    void onPush() override;
    void onPop() override;
    void onTick(float dt) override;
    void onRender() override;
    void onSizeEvent(const WindowSizeEvent &event);

   private:
    RenderTarget *m_target;
    OrthographicCamera m_camera;
    FpsCounter m_fps;
    Ref<Font> m_font;
};

}  // namespace SD

#endif /* SD_PROFILE_SYSTEM_HPP */
