#include "Renderer/Renderer.hpp"
#include "Renderer/SpriteRenderer.hpp"
#include "Renderer/MeshRenderer.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Device.hpp"
#include "Utility/String.hpp"

#include "Loader/ShaderLoader.hpp"

namespace SD {

Device* Renderer::m_device;

Ref<UniformBuffer> Renderer::m_camera_UBO;
CameraData Renderer::m_camera_data;

Ref<VertexArray> Renderer::m_quad_vao;
Ref<VertexBuffer> Renderer::m_quad_vbo;
Ref<IndexBuffer> Renderer::m_quad_ibo;

Ref<VertexArray> Renderer::m_box_vao;
Ref<VertexBuffer> Renderer::m_box_vbo;
Ref<IndexBuffer> Renderer::m_box_ibo;

static std::stack<Framebuffer*> s_buffer_stacks;
Framebuffer* s_current_buffer;

void Renderer::BeginRenderPass(const RenderPassInfo& info) {
    s_buffer_stacks.push(info.framebuffer);
    s_current_buffer = GetBufferStackTop();
    m_device->SetFramebuffer(s_current_buffer);
    m_device->SetViewport(0, 0, s_current_buffer->GetWidth(),
                          s_current_buffer->GetHeight());
}

void Renderer::BeginRenderSubpass(const RenderSubpassInfo& info) {
    s_current_buffer = GetBufferStackTop();
    m_device->SetFramebuffer(s_current_buffer);
    m_device->DrawBuffers(s_current_buffer, info.draw_buffer_count,
                          info.draw_buffer);
}

void Renderer::EndRenderSubpass() { m_device->SetFramebuffer(nullptr); }

void Renderer::EndRenderPass() { s_buffer_stacks.pop(); }

Framebuffer* Renderer::GetBufferStackTop() { return s_buffer_stacks.top(); }

glm::vec2 Renderer::GetCurrentBufferSize() {
    return {s_current_buffer->GetWidth(), s_current_buffer->GetHeight()};
}

bool Renderer::IsEmptyStack() { return s_buffer_stacks.empty(); }

void Renderer::Init(Device* device) {
    m_device = device;
    SD_CORE_TRACE("Initializing Renderer");
    m_camera_UBO = UniformBuffer::Create(nullptr, sizeof(CameraData),
                                         BufferIOType::DYNAMIC);

    // NDC quad
    {
        const float quad_vert[] = {
            -1.0f, -1.0f, 0.f, 0.f,  0.f,   // bottom left
            1.0f,  -1.0f, 0.f, 1.0f, 0.f,   // bottom right
            1.0f,  1.0f,  0.f, 1.0f, 1.0f,  // top right
            -1.0f, 1.0f,  0.f, 0.f,  1.0f,  // top left
        };
        const uint32_t indices[] = {0, 1, 2, 2, 3, 0};
        m_quad_vbo = VertexBuffer::Create(quad_vert, sizeof(quad_vert),
                                          BufferIOType::STATIC);
        m_quad_ibo = IndexBuffer::Create(indices, 6, BufferIOType::STATIC);
        m_quad_vao = VertexArray::Create();
        VertexBufferLayout layout;
        layout.Push(BufferLayoutType::FLOAT3);
        layout.Push(BufferLayoutType::FLOAT2);
        m_quad_vao->AddBufferLayout(layout);
        m_quad_vao->BindVertexBuffer(*m_quad_vbo, 0);
        m_quad_vao->BindIndexBuffer(*m_quad_ibo);
    }

    // NDC box
    {
        const float box_vert[] = {
            // front
            -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, -1.0,
            // back
            -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 1.0};

        const uint32_t box_indices[] = {// front
                                        0, 1, 2, 2, 3, 0,
                                        // right
                                        1, 5, 6, 6, 2, 1,
                                        // back
                                        7, 6, 5, 5, 4, 7,
                                        // left
                                        4, 0, 3, 3, 7, 4,
                                        // bottom
                                        4, 5, 1, 1, 0, 4,
                                        // top
                                        3, 2, 6, 6, 7, 3};
        m_box_vao = VertexArray::Create();

        VertexBufferLayout layout;
        layout.Push(BufferLayoutType::FLOAT3);
        m_box_vbo = VertexBuffer::Create(box_vert, sizeof(box_vert),
                                         BufferIOType::STATIC);
        m_box_ibo = IndexBuffer::Create(box_indices, 36, BufferIOType::STATIC);
        m_box_vao->AddBufferLayout(layout);
        m_box_vao->BindVertexBuffer(*m_box_vbo, 0);
        m_box_vao->BindIndexBuffer(*m_box_ibo);
    }
    SpriteRenderer::Init();
    MeshRenderer::Init();
}

void Renderer::Submit(const VertexArray& vao, MeshTopology topology,
                      size_t count, size_t offset, bool index) {
    m_device->SetVertexArray(&vao);
    if (index) {
        m_device->DrawElements(topology, count, offset);
    } else {
        m_device->DrawArrays(topology, offset, count);
    }
}

void Renderer::DrawNDCQuad() {
    Submit(*m_quad_vao, MeshTopology::TRIANGLES, m_quad_ibo->GetCount(), 0);
}

void Renderer::DrawNDCBox() {
    Submit(*m_box_vao, MeshTopology::TRIANGLES, m_box_ibo->GetCount(), 0);
}

}  // namespace SD
