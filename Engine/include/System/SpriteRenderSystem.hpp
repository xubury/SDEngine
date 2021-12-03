#ifndef SD_SPRITE_RENDER_SYSTEM_HPP
#define SD_SPRITE_RENDER_SYSTEM_HPP

#include "System/Export.hpp"
#include "Core/System.hpp"

namespace SD {

class SD_SYSTEM_API SpriteRenderSystem : public System {
   public:
    SpriteRenderSystem(RenderTarget *target);

    void OnPush() override;

    void OnPop() override;

    void OnRender() override;

   private:
    RenderTarget *m_target;
};

}  // namespace SD

#endif