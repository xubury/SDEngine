#ifndef SD_SPRITE_RENDER_SYSTEM_HPP
#define SD_SPRITE_RENDER_SYSTEM_HPP

#include "Renderer/Export.hpp"
#include "Core/System.hpp"

namespace SD {

class SD_RENDERER_API SpriteRenderSystem : public System {
   public:
    SpriteRenderSystem();

    void OnTick(float dt) override;

    void OnPush() override;

    void OnPop() override;

    void OnRender() override;

   private:
};

}  // namespace SD

#endif
