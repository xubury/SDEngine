#ifndef SD_SPRITE_RENDER_SYSTEM_HPP
#define SD_SPRITE_RENDER_SYSTEM_HPP

#include "Renderer/System/System.hpp"
#include "Renderer/System/Event.hpp"

namespace sd {

class SD_API SpriteRenderSystem : public System {
   public:
    SpriteRenderSystem();

    void onInit() override;

    void onDestroy() override;

    void onRender() override;

   private:
};

}  // namespace sd

#endif
