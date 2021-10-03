#ifndef SD_PROFILE_SYSTEM_HPP
#define SD_PROFILE_SYSTEM_HPP

#include "ECS/System.hpp"
#include "Core/Timing.hpp"

namespace sd {

class RenderEngine;

class ProfileSystem : public System {
   public:
    ProfileSystem();

    void onTick(float dt) override;
    void onRender() override;
    void onResize(int width, int height) override;

   private:
    OrthographicCamera m_camera;
    FpsCounter m_fps;
};

}  // namespace sd

#endif /* SD_PROFILE_SYSTEM_HPP */
