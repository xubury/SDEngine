#ifndef SD_PROFILE_SYSTEM_HPP
#define SD_PROFILE_SYSTEM_HPP

#include "Core/System.hpp"
#include "Core/Timing.hpp"
#include "Graphics/Font.hpp"

namespace sd {

class ProfileSystem : public System {
   public:
    ProfileSystem(int width, int height);

    void onInit() override;
    void onDestroy() override;
    void onRender() override;
    void onSizeEvent(const SizeEvent &event);

   private:
    OrthographicCamera m_camera;
    FpsCounter m_fps;
    Ref<Font> m_font;
};

}  // namespace sd

#endif /* SD_PROFILE_SYSTEM_HPP */
