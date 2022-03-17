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

struct RenderPassInfo {
    Framebuffer *framebuffer;
};

struct RenderSubpassInfo {
    const int *draw_buffer;
    int draw_buffer_count;
};

class SD_RENDERER_API Renderer {
   public:
    static void BeginRenderPass(const RenderPassInfo &info);
    static void BeginRenderSubpass(const RenderSubpassInfo &info);
    static void EndRenderSubpass();
    static void EndRenderPass();
    static void Submit(const VertexArray &vao, MeshTopology topology,
                       size_t count, size_t offset, bool index = true);

    static void Init(Device *device);

    static glm::vec2 GetCurrentBufferSize();
    static Framebuffer *GetBufferStackTop();

    static bool IsEmptyStack();

    static void DrawNDCQuad();
    static void DrawNDCBox();
   protected:
    static Device *m_device;

    static Ref<UniformBuffer> m_camera_UBO;

    static CameraData m_camera_data;

    static Ref<VertexArray> m_quad_vao;
    static Ref<VertexBuffer> m_quad_vbo;
    static Ref<IndexBuffer> m_quad_ibo;

    static Ref<VertexArray> m_box_vao;
    static Ref<VertexBuffer> m_box_vbo;
    static Ref<IndexBuffer> m_box_ibo;
};

}  // namespace SD

#endif /* SD_RENDERER_HPP */
