#ifndef SD_RENDERER_API_HPP
#define SD_RENDERER_API_HPP

#include <cstdint>
#include "Graphics/GraphicsDefine.hpp"

namespace sd {

class Framebuffer;

// just a shortcut to Device
class Renderer {
   public:
    static void init(API api);

    static API getAPI();

    static void drawElements(MeshTopology topology, size_t count,
                             size_t offset);

    static void setClearColor(float r, float g, float b, float a);

    static void clear();

    static void setViewport(int x, int y, int width, int height);

    static void setFramebuffer(const Framebuffer *framebuffer);

    Renderer() = delete;

};  // namespace Renderer

}  // namespace sd

#endif /* SD_RENDERER_API_HPP */
