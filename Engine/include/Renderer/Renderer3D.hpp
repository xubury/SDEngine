#ifndef SD_RENDERER_3D_HPP
#define SD_RENDERER_3D_HPP

#include "Graphics/Graphics.hpp"

namespace sd {

class Camera;

class RenderTarget;

class SD_API Renderer3D {
   public:
    static void init();
    static void beginScene(const Camera &camera, const RenderTarget &target);
    static void endScene();

   private:
    Renderer3D() = default;
};

}  // namespace sd

#endif /* SD_RENDERER_3D_HPP */
