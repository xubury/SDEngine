#ifndef SD_DEVICE_HPP
#define SD_DEVICE_HPP

#include "Utility/Base.hpp"
#include "Graphics/Graphics.hpp"

namespace SD {

class VertexArray;
class Shader;
class Framebuffer;

class SD_GRAPHICS_API Device {
   public:
    enum class API { OpenGL, Vulkan, Direct3D };

    static API GetAPI();
    static void SetAPI(API api);

    static Scope<Device> Create();

    Device() = default;
    virtual ~Device() = default;

    Device(const Device &) = delete;

    Device &operator=(const Device &) = delete;

    virtual void DispatchCompute(int32_t num_group_x, int32_t num_group_y,
                                 int32_t num_group_z) = 0;
    // TODO
    virtual void MemoryBarrier() = 0;

    virtual void DrawElements(MeshTopology topology, int count,
                              size_t offset) = 0;
    virtual void DrawElementsInstanced(MeshTopology topology, int count,
                                       size_t offset, size_t amount) = 0;

    virtual void DrawArrays(MeshTopology topology, int first, int count) = 0;

    virtual void SetLineWidth(float width) = 0;

    virtual void SetClearColor(float r, float g, float b, float a) = 0;

    virtual void Clear(BufferBitMask bit = BufferBitMask::ColorBufferBit |
                                           BufferBitMask::DepthBufferBit |
                                           BufferBitMask::StencilBufferBit) = 0;

    virtual void SetVertexArray(const VertexArray *vertex_array) = 0;

    virtual void SetShader(const Shader *shader) = 0;

    virtual void SetViewport(int x, int y, int width, int height) = 0;

    virtual void SetFramebuffer(const Framebuffer *framebuffer) = 0;

    virtual void SetPolygonMode(PolygonMode mode, Face face) = 0;

    virtual void SetDepthMask(bool depth_mask) = 0;

    virtual void Enable(Operation operation) = 0;

    virtual void Disable(Operation operation) = 0;

    virtual void SetCullFace(Face face) = 0;

    virtual void SetDepthfunc(DepthFunc depth_func) = 0;

    virtual void DrawBuffer(Framebuffer *fb, int buf) = 0;
    virtual void DrawBuffers(Framebuffer *fb, int n, const int *buf) = 0;

    virtual void ReadBuffer(const Framebuffer *fb, int buf) = 0;

    virtual void BlitFramebuffer(const Framebuffer *src, int src_x, int src_y,
                                 int src_width, int src_height,
                                 Framebuffer *dst, int dst_x, int dst_y,
                                 int dst_width, int dst_height,
                                 BufferBitMask mask, BlitFilter filter) = 0;
};

}  // namespace SD

#endif /* SD_DEVICE_HPP */
