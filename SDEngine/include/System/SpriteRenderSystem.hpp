#ifndef SD_SPRITE_RENDER_SYSTEM_HPP
#define SD_SPRITE_RENDER_SYSTEM_HPP

#include "System/Export.hpp"
#include "Core/System.hpp"

namespace SD {

class SD_SYSTEM_API SpriteRenderSystem : public System {
   public:
    SpriteRenderSystem();

    void OnPush() override;

    void OnPop() override;

    void OnRender() override;
};

}  // namespace SD

#endif