#ifndef SD_RENDERER_3D_HPP
#define SD_RENDERER_3D_HPP

#include "Renderer/Renderer.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Mesh.hpp"
#include "Utils/Transform.hpp"

namespace sd {

class Camera;

class RenderTarget;

class SD_API Renderer3D {
   public:
    static void init();
    static void beginScene(Camera &camera);

    static void endScene();

    static void drawMesh(const Mesh &mesh);

   private:
    Renderer3D() = default;
};

}  // namespace sd

#endif /* SD_RENDERER_3D_HPP */
