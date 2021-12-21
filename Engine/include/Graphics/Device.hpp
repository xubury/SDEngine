#ifndef SD_DEVICE_HPP
#define SD_DEVICE_HPP

#include "Utility/Base.hpp"
#include "Graphics/RenderTarget.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Context.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/Framebuffer.hpp"

namespace SD {

class SD_GRAPHICS_API Device {
   public:
    static Ref<Device> Create(Context *context);

    virtual ~Device() = default;

    Device(const Device &) = delete;

    Device &operator=(const Device &) = delete;

    virtual void DrawElements(MeshTopology topology, int count,
                              size_t offset) = 0;

    virtual void DrawArrays(MeshTopology topology, int first, int count) = 0;

    virtual void SetLineWidth(float width) = 0;

    virtual void SetClearColor(float r, float g, float b, float a) = 0;

    virtual void Clear(
        BufferBitMask bit = BufferBitMask::COLOR_BUFFER_BIT |
                            BufferBitMask::DEPTH_BUFFER_BIT |
                            BufferBitMask::STENCIL_BUFFER_BIT) = 0;

    void SetTarget(RenderTarget &target);

    virtual void SetViewport(const Viewport &viewport) = 0;

    virtual Viewport GetViewport(const RenderTarget &target) = 0;

    virtual void SetFramebuffer(Framebuffer *framebuffer) = 0;

    virtual void SetPolygonMode(PolygonMode mode, Face face) = 0;

    virtual void SetDepthMask(bool depth_mask) = 0;

    virtual void Enable(Operation operation) = 0;

    virtual void Disable(Operation operation) = 0;

    virtual void SetCullFace(Face face) = 0;

    virtual void SetDepthfunc(DepthFunc depth_func) = 0;

    virtual void ResetShaderState() = 0;

    virtual void BlitFramebuffer(Framebuffer *src, uint32_t src_attachment,
                                 Framebuffer *dst, uint32_t dst_attachment,
                                 BufferBitMask mask, BlitFilter filter) = 0;

    virtual const glm::ivec2 GetUVIndex(int index) const = 0;

    glm::ivec2 GetSize() const;
    int GetWidth() const;
    int GetHeight() const;

    uint8_t GetMSAA() const;

   protected:
    Device(Context *context);

    Context *m_context;
};

}  // namespace SD

#endif /* SD_DEVICE_HPP */
