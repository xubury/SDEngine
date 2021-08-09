#ifndef SD_RENDERER_API_HPP
#define SD_RENDERER_API_HPP

#include <cstdint>
#include "Graphics/Graphics.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"

namespace sd {

class Framebuffer;

// just a shortcut to Device
class SD_API Renderer {
   public:
    static void init(API api);

    static API getAPI();

    static void submit(const VertexArray &vao, MeshTopology topology,
                       size_t count, size_t offset);

    static void setClearColor(float r, float g, float b, float a);

    static void clear();

    static void setViewport(int x, int y, int width, int height);

    static void setFramebuffer(const Framebuffer *framebuffer);

   private:
    Renderer() = default;

};  // namespace Renderer

}  // namespace sd

#endif /* SD_RENDERER_API_HPP */
