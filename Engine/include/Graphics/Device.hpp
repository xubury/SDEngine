#ifndef SD_DEVICE_HPP
#define SD_DEVICE_HPP

#include "Utility/Base.hpp"
#include "Graphics/Graphics.hpp"

namespace sd {

class Framebuffer;

class SD_API Device {
   public:
    static void init();

    static Device &instance();

    virtual ~Device() = default;

    Device(const Device &) = delete;

    Device &operator=(const Device &) = delete;

    virtual void drawElements(MeshTopology topology, size_t count,
                              size_t offset) = 0;

    virtual void setClearColor(float r, float g, float b, float a) = 0;

    virtual void clear(
        BufferBitMask bit = BufferBitMask::COLOR_BUFFER_BIT |
                            BufferBitMask::DEPTH_BUFFER_BIT |
                            BufferBitMask::STENCIL_BUFFER_BIT) = 0;

    virtual void setViewport(int x, int y, int width, int height) = 0;

    virtual void setFramebuffer(const Framebuffer *framebuffer) = 0;

    virtual void setPolygonMode(PolygonMode mode, Face face) = 0;

    virtual void setDepthMask(bool depthMask) = 0;

    virtual void enable(Operation operation) = 0;

    virtual void disable(Operation operation) = 0;

    virtual void setCullFace(Face cullFace) = 0;

    virtual void setDepthfunc(DepthFunc depthFunc) = 0;

    virtual void resetShaderState() = 0;

   protected:
    Device() = default;
};

}  // namespace sd

#endif /* SD_DEVICE_HPP */
