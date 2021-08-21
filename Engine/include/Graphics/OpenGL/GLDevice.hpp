#ifndef SD_GL_DEVICE_HPP
#define SD_GL_DEVICE_HPP

#include "Graphics/Device.hpp"

namespace sd {

class GLDevice : public Device {
   public:
    void drawElements(MeshTopology topology, size_t count,
                      size_t offset) override;

    void setClearColor(float r, float g, float b, float a) override;

    void clear() override;

    void setViewport(int x, int y, int width, int height) override;

    void setFramebuffer(const Framebuffer *framebuffer) override;

    void setWireframe(bool wireframe) override;
};

}  // namespace sd

#endif /* SD_GL_DEVICE_HPP */
