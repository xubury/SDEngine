#ifndef SD_DEVICE_HPP
#define SD_DEVICE_HPP

#include "Common/Base.hpp"
#include "Graphics/Graphics.hpp"

namespace sd {

class Framebuffer;

class SD_API Device {
   public:
    virtual ~Device() = default;

   protected:
    Device(const Device &) = delete;

    Device &operator=(const Device &) = delete;

    virtual void drawElements(MeshTopology topology, size_t count,
                              size_t offset) = 0;

    virtual void setClearColor(float r, float g, float b, float a) = 0;

    virtual void clear() = 0;

    virtual void setViewport(int x, int y, int width, int height) = 0;

    virtual void setFramebuffer(const Framebuffer *framebuffer) = 0;

    virtual void setWireframe(bool wireframe) = 0;

    virtual void setDepthMask(bool depthMask) = 0;

    virtual void setBlend(bool blend) = 0;

    friend class Renderer;

    Device() = default;
};

}  // namespace sd

#endif /* SD_DEVICE_HPP */
