#ifndef SD_RENDERER_HPP
#define SD_RENDERER_HPP

#include <cstdint>
#include "Graphics/Graphics.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/VertexArray.hpp"
#include "Renderer/RenderTarget.hpp"

namespace sd {

class SD_API Renderer {
   public:
    static void init();

    static void submit(const VertexArray &vao, MeshTopology topology,
                       size_t count, size_t offset);

    static RenderTarget &getDefaultTarget();

    static void setDefaultTarget(const RenderTarget &target);

    static void setShader(Shader &shader);

    static void setCamera(Camera &camera);

   private:
    Renderer() = default;

    static Renderer &instance();

    RenderTarget m_defaultTarget;

};  // namespace Renderer

}  // namespace sd

#endif /* SD_RENDERER_HPP */
