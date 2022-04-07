#ifndef SD_RENDERER_HPP
#define SD_RENDERER_HPP

#include "Graphics/Light.hpp"
#include "Renderer/Export.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Mesh.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/Font.hpp"

namespace SD {

class Scene;

struct SD_RENDERER_API CameraData {
    Matrix4f projection;
    Matrix4f view;
};

struct RenderOperation {
    bool depth_test{true};
    bool depth_mask{true};

    DepthFunc depth_func{DepthFunc::Less};

    bool blend{true};

    bool face_culling{true};
    Face cull_face{Face::Back};
};

struct RendererData {
    Ref<UniformBuffer> camera_ubo;
    CameraData camera_data;

    Scene *scene;

    Ref<VertexArray> quad_vao;
    Ref<VertexBuffer> quad_vbo;
    Ref<IndexBuffer> quad_ibo;

    Ref<VertexArray> box_vao;
    Ref<VertexBuffer> box_vbo;
    Ref<IndexBuffer> box_ibo;

    bool is_subpass_begin{false};
};

struct RenderPassInfo {
    Framebuffer *framebuffer;
    int32_t viewport_width;
    int32_t viewport_height;

    RenderOperation op{};
    BufferBitMask clear_mask{BufferBitMask::ColorBufferBit |
                             BufferBitMask::DepthBufferBit};
    std::array<float, 4> clear_value{0.f, 0.f, 0.f, 1.0f};
};

struct RenderSubpassInfo {
    const int *draw_buffer;
    int draw_buffer_count;
    RenderOperation op{};
};

class SD_RENDERER_API Renderer {
   public:
    static void BeginRenderPass(const RenderPassInfo &info);
    static void BeginRenderSubpass(const RenderSubpassInfo &info);
    static void EndRenderSubpass();
    static void EndRenderPass();
    static void Submit(const Shader &shader, const VertexArray &vao,
                       MeshTopology topology, size_t count, size_t offset,
                       bool index = true);
    static void ComputeImage(const Shader &shader, int32_t width,
                             int32_t height, int32_t depth);

    static void Init(Device *device);

    static Framebuffer *GetBufferStackTop();
    static const RenderPassInfo &GetCurrentRenderPass();

    static bool IsEmptyStack();

    static void SetCamera(Camera &camera);

    static void BindCamera(Shader &shader);

    static void DrawNDCQuad(const Shader &shader);
    static void DrawNDCBox(const Shader &shader);

    // blit current renderpass framebuffer content to draw_fb
    static void BlitToBuffer(int read_attachment, Framebuffer *draw_fb,
                             int draw_attachment, BufferBitMask mask);
    // blit read_fb content to current renderpass's framebuffer
    static void BlitFromBuffer(int draw_attachment, Framebuffer *read_fb,
                               int read_attachment, BufferBitMask mask);

   protected:
    static void SetRenderState(Framebuffer *framebuffer, int32_t viewport_width,
                               int32_t viewport_height,
                               const RenderOperation &op);
    static Device *s_device;
    static RendererData s_data;
};

}  // namespace SD

#endif /* SD_RENDERER_HPP */
