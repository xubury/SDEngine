#ifndef SD_SPRITE_RENDER_SYSTEM_HPP
#define SD_SPRITE_RENDER_SYSTEM_HPP

#include "Core/System.hpp"

namespace SD {

class SD_API SpriteRenderSystem : public System {
   public:
    SpriteRenderSystem(RenderTarget *target);

    void onInit() override;

    void onDestroy() override;

    void onRender() override;

   private:
    RenderTarget *m_target;
};

}  // namespace SD

#endif
