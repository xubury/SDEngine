#ifndef SD_GL_DEVICE_HPP
#define SD_GL_DEVICE_HPP

#include "Graphics/Device.hpp"

namespace SD {

class GLDevice : public Device {
   public:
    GLDevice(Context *context);

    void DrawElements(MeshTopology topology, int count, size_t offset) override;

    void DrawArrays(MeshTopology topology, int first, int count) override;

    void SetLineWidth(float width) override;

    void SetClearColor(float r, float g, float b, float a) override;

    void Clear(BufferBitMask bit) override;

    Viewport GetViewport(const RenderTarget &target) override;

    void SetViewport(const Viewport &viewport) override;

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
                         BufferBitMask mask, BlitFilter filter) override;

    const glm::ivec2 GetUVIndex(int index) const override;
};

}  // namespace SD

#endif /* SD_GL_DEVICE_HPP */
