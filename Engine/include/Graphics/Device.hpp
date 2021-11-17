#ifndef SD_DEVICE_HPP
#define SD_DEVICE_HPP

#include "Utility/Base.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/Framebuffer.hpp"

namespace SD {

class SD_API Device {
   public:
    static Ref<Device> Create();

    virtual ~Device() = default;

    Device(const Device &) = delete;

    Device &operator=(const Device &) = delete;

    virtual void DrawElements(MeshTopology topology, size_t count,
                              size_t offset) = 0;

    virtual void SetClearColor(float r, float g, float b, float a) = 0;

    virtual void Clear(
        BufferBitMask bit = BufferBitMask::COLOR_BUFFER_BIT |
                            BufferBitMask::DEPTH_BUFFER_BIT |
                            BufferBitMask::STENCIL_BUFFER_BIT) = 0;

    virtual void SetViewport(int x, int y, int width, int height) = 0;

    virtual void SetFramebuffer(Framebuffer *framebuffer) = 0;

    virtual void SetPolygonMode(PolygonMode mode, Face face) = 0;

    virtual void SetDepthMask(bool depthMask) = 0;

    virtual void Enable(Operation operation) = 0;

    virtual void Disable(Operation operation) = 0;

    virtual void SetCullFace(Face cullFace) = 0;

    virtual void SetDepthfunc(DepthFunc depthFunc) = 0;

    virtual void ResetShaderState() = 0;

    virtual void BlitFramebuffer(Framebuffer *src, uint32_t srcAttachment,
                                 Framebuffer *dst, uint32_t dstAttachment,
                                 BufferBitMask mask, TextureFilter filter) = 0;

   protected:
    Device() = default;
};

}  // namespace SD

#endif /* SD_DEVICE_HPP */
