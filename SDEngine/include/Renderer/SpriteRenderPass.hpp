#ifndef SD_SPRITE_RENDERER_HPP
#define SD_SPRITE_RENDERER_HPP

#include "Renderer/Export.hpp"
#include "ECS/Scene.hpp"
#include "Resource/Resource.hpp"
#include "Graphics/Camera.hpp"

namespace SD {

class SD_RENDERER_API SpriteRenderPass {
   public:
    static void Init(TextureCache &textures);
    static void Render(const Scene &scene);
};

}  // namespace SD

#endif
