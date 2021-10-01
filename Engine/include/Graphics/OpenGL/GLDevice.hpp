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

    void clear() override;

    void clearDepth() override;

    void setViewport(int x, int y, int width, int height) override;

    void setFramebuffer(const Framebuffer *framebuffer) override;

    void setWireframe(bool wireframe) override;

    void setDepthMask(bool depthMask) override;

    void setBlend(bool blend) override;

    void setDepthTest(bool depthTest) override;

    void setMultisample(bool multisample) override;

    void setCullFace(CullFace cullface) override;
};

}  // namespace sd

#endif /* SD_GL_DEVICE_HPP */
