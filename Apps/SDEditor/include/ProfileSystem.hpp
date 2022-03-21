#ifndef SD_PROFILE_SYSTEM_HPP
#define SD_PROFILE_SYSTEM_HPP

#include "Core/ECSSystem.hpp"
#include "Graphics/Font.hpp"
#include "Graphics/Camera.hpp"
#include "Utility/Timing.hpp"

namespace SD {

class ProfileSystem : public ECSSystem {
   public:
    ProfileSystem();

    void OnInit() override;

    void OnPush() override;
    void OnPop() override;

    void OnTick(float dt) override;
    void OnRender() override;

   private:
    Camera m_camera;
    FPSCounter m_fps;
    Ref<Font> m_font;

    bool m_is_show_message;

    HandlerRegistration m_size_handler;
};

}  // namespace SD

#endif /* SD_PROFILE_SYSTEM_HPP */
