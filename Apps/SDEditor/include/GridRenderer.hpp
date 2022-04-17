#ifndef SD_GRID_RENDERER_HPP
#define SD_GRID_RENDERER_HPP

#include "Utility/Math.hpp"
#include "ECS/Scene.hpp"

namespace SD {

class SpriteFrame;
class Camera;
class TileBrush;

class GridRenderer {
   public:
    static void Init();
    static void Render(const Camera &camera, const SpriteFrame &frame,
                       const TileBrush &brush);

   private:
};

}  // namespace SD
#endif
