#ifndef SD_GL_DEVICE_HPP
#define SD_GL_DEVICE_HPP

#include "Graphics/Device.hpp"

namespace SD {

class GLDevice : public Device {
   public:
    GLDevice();

    void DrawElements(MeshTopology topology, size_t count,
                      size_t offset) override;

    void SetClearColor(float r, float g, float b, float a) override;

    void Clear(BufferBitMask bit) override;

    void SetViewport(int x, int y, int width, int height) override;

    void SetFramebuffer(Framebuffer *framebuffer) override;

    void SetPolygonMode(PolygonMode mode, Face face) override;

    void SetDepthMask(bool depth_mask) override;

    void Enable(Operation operation) override;

    void Disable(Operation operation) override;

    void SetCullFace(Face face) override;

    void SetDepthfunc(DepthFunc depth_func) override;

    void ResetShaderState() override;

    void BlitFramebuffer(Framebuffer *src, uint32_t src_attachment,
                         Framebuffer *dst, uint32_t dst_attachment,
                         BufferBitMask mask, TextureMagFilter filter) override;
};

}  // namespace SD

#endif /* SD_GL_DEVICE_HPP */
