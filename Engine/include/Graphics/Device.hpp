#ifndef SD_DEVICE_HPP
#define SD_DEVICE_HPP

#include "Utility/Base.hpp"
#include "Graphics/Graphics.hpp"

namespace sd {

class Framebuffer;

class SD_API Device {
   public:
    static void create();

    static Device &instance();

    virtual ~Device() = default;

    Device(const Device &) = delete;

    Device &operator=(const Device &) = delete;

    virtual void init() = 0;

    virtual void drawElements(MeshTopology topology, size_t count,
                              size_t offset) = 0;

    virtual void setClearColor(float r, float g, float b, float a) = 0;

    virtual void clear() = 0;

    virtual void clearDepth() = 0;

    virtual void setViewport(int x, int y, int width, int height) = 0;

    virtual void setFramebuffer(const Framebuffer *framebuffer) = 0;

    virtual void setWireframe(bool wireframe) = 0;

    virtual void setDepthMask(bool depthMask) = 0;

    virtual void setBlend(bool blend) = 0;

    virtual void setDepthTest(bool depthTest) = 0;

    virtual void setMultisample(bool multisample) = 0;

    virtual void setCullFace(CullFace cullFace) = 0;

   protected:
    Device() = default;
};

}  // namespace sd

#endif /* SD_DEVICE_HPP */
