#ifndef SD_GL_DEVICE_HPP
#define SD_GL_DEVICE_HPP

#include "Graphics/Device.hpp"

namespace sd {

class GLDevice : public Device {
   public:
    void init() override;

    void drawElements(MeshTopology topology, size_t count,
                      size_t offset) override;

    void setClearColor(float r, float g, float b, float a) override;

    void clear(BufferBitMask bit) override;

    void setViewport(int x, int y, int width, int height) override;

    void setFramebuffer(const Framebuffer *framebuffer) override;

    void setPolygonMode(PolygonMode mode, Face face) override;

    void setDepthMask(bool depthMask) override;

    void enable(Operation operation) override;

    void disable(Operation operation) override;

    void setCullFace(Face cullface) override;
};

}  // namespace sd

#endif /* SD_GL_DEVICE_HPP */
