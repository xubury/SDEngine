#ifndef SD_RENDERER_3D_HPP
#define SD_RENDERER_3D_HPP

#include "Graphics/Graphics.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"

namespace sd {

class Camera;

class RenderTarget;

class SD_API Renderer3D {
   public:
    static void init();
    static void beginScene(const Camera &camera, const RenderTarget &target);
    static void endScene();

    static void submit(const Shader &shader, const VertexArray &vao,
                       MeshTopology topology, size_t count);

   private:
    Renderer3D() = default;
};

}  // namespace sd

#endif /* SD_RENDERER_3D_HPP */
