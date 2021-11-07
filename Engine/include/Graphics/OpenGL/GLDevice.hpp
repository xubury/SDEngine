#ifndef SD_GL_DEVICE_HPP
#define SD_GL_DEVICE_HPP

#include "Graphics/Device.hpp"

namespace sd {

class GLDevice : public Device {
   public:
    GLDevice();

    void drawElements(MeshTopology topology, size_t count,
                      size_t offset) override;

    void setClearColor(float r, float g, float b, float a) override;

    void clear(BufferBitMask bit) override;

    void setViewport(int x, int y, int width, int height) override;

    void setFramebuffer(Framebuffer *framebuffer) override;

    void setPolygonMode(PolygonMode mode, Face face) override;

    void setDepthMask(bool depthMask) override;

    void enable(Operation operation) override;

    void disable(Operation operation) override;

    void setCullFace(Face cullface) override;

    void setDepthfunc(DepthFunc depthFunc) override;

    void resetShaderState() override;

    void blitFramebuffer(Framebuffer *src, uint32_t srcAttachment,
                         Framebuffer *dst, uint32_t dstAttachment,
                         BufferBitMask mask, TextureFilter filter) override;
};

}  // namespace sd

#endif /* SD_GL_DEVICE_HPP */
