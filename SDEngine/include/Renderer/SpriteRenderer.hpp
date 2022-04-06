#ifndef SD_SPRITE_RENDERER_HPP
#define SD_SPRITE_RENDERER_HPP

#include "Renderer/Export.hpp"
#include "ECS/Scene.hpp"
#include "Graphics/Camera.hpp"

namespace SD {

class SD_RENDERER_API SpriteRenderer {
   public:
    SpriteRenderer() = default;

    void Render();
};

}  // namespace SD

#endif
