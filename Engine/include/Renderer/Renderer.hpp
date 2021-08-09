#ifndef SD_RENDERER_API_HPP
#define SD_RENDERER_API_HPP

#include <cstdint>
#include "Graphics/Graphics.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"
#include "Renderer/RenderTarget.hpp"

namespace sd {

class Framebuffer;

class SD_API Renderer {
   public:
    static void init();

    static void submit(const VertexArray &vao, MeshTopology topology,
                       size_t count, size_t offset);

    static void setClearColor(float r, float g, float b, float a);

    static void clear();

    static void setViewport(int x, int y, int width, int height);

    static void setFramebuffer(const Framebuffer *framebuffer);

    static RenderTarget &getDefaultTarget();

    static void setDefaultTarget(Ref<RenderTarget> target);

   private:
    Renderer() = default;

    static Renderer &instance();

    Ref<RenderTarget> m_defaultTarget;
    API m_api;

};  // namespace Renderer

}  // namespace sd

#endif /* SD_RENDERER_API_HPP */
