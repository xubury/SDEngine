#ifndef SD_PROFILE_SYSTEM_HPP
#define SD_PROFILE_SYSTEM_HPP

#include "Core/System.hpp"
#include "Core/Timing.hpp"

namespace sd {

class RenderEngine;

class ProfileSystem : public System {
   public:
    ProfileSystem();

    void onInit() override;
    void onTick(float dt) override;
    void onRender() override;
    void onSizeEvent(const SizeEvent &event);

   private:
    OrthographicCamera m_camera;
    FpsCounter m_fps;
};

}  // namespace sd

#endif /* SD_PROFILE_SYSTEM_HPP */
