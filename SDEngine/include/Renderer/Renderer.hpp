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
    glm::mat4 projection;
    glm::mat4 view;
};

struct RenderOperation {
    bool depth_test{true};
    bool depth_mask{true};

    DepthFunc depth_func{DepthFunc::Less};

    bool blend{true};

    bool face_culling{true};
    Face cull_face{Face::Back};
};

struct RenderPassInfo {
    Framebuffer *framebuffer;
    int32_t viewport_width;
    int32_t viewport_height;

    RenderOperation op{};
    BufferBitMask clear_mask{BufferBitMask::ColorBufferBit |
                             BufferBitMask::DepthBufferBit};
    float clear_value[4] = {1.0f, 1.0f, 1.0f, 1.0f};
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
    static void Submit(const VertexArray &vao, MeshTopology topology,
                       size_t count, size_t offset, bool index = true);

    static void Init();

    static glm::vec2 GetCurrentBufferSize();
    static Framebuffer *GetBufferStackTop();
    static const RenderPassInfo &GetCurrentRenderPass();

    static bool IsEmptyStack();

    static void SetCamera(Shader &shader, Camera &camera);

    static void DrawNDCQuad(const Shader &shader);
    static void DrawNDCBox(const Shader &shader);

    // blit current renderpass framebuffer content to draw_fb
    static void BlitToBuffer(int read_attachment, Framebuffer *draw_fb,
                             int draw_attachment, BufferBitMask mask);
    // blit read_fb content to current renderpass's framebuffer
    static void BlitFromBuffer(int draw_attachment, Framebuffer *read_fb,
                               int read_attachment, BufferBitMask mask);

   protected:
    static Scope<Device> m_device;

    static Ref<UniformBuffer> m_camera_UBO;
    static CameraData m_camera_data;

   private:
    static void SetRenderState(Framebuffer *framebuffer, int32_t viewport_width,
                               int32_t viewport_height,
                               const RenderOperation &op);

    static Ref<VertexArray> m_quad_vao;
    static Ref<VertexBuffer> m_quad_vbo;
    static Ref<IndexBuffer> m_quad_ibo;

    static Ref<VertexArray> m_box_vao;
    static Ref<VertexBuffer> m_box_vbo;
    static Ref<IndexBuffer> m_box_ibo;

    static bool s_is_subpass_begin;
};

}  // namespace SD

#endif /* SD_RENDERER_HPP */
