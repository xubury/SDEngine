#ifndef SD_PROFILE_SYSTEM_HPP
#define SD_PROFILE_SYSTEM_HPP

#include "System/Export.hpp"
#include "Core/System.hpp"
#include "Asset/Font.hpp"
#include "Renderer/Camera.hpp"
#include "Utility/Timing.hpp"

namespace SD {

class SD_SYSTEM_API ProfileSystem : public System {
   public:
    ProfileSystem(RenderTarget *target, int width, int height);

    void OnInit() override;

    void OnPush() override;
    void OnPop() override;

    void OnTick(float dt) override;
    void OnRender() override;

   private:
    RenderTarget *m_target;
    Camera m_camera;
    FPSCounter m_fps;
    Ref<Font> m_font;

    bool m_is_show_message;

    HandlerRegistration m_size_handler;
};

}  // namespace SD

#endif /* SD_PROFILE_SYSTEM_HPP */
