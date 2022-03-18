#include "Renderer/Renderer.hpp"
#include "Renderer/SpriteRenderer.hpp"
#include "Renderer/MeshRenderer.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Device.hpp"
#include "Utility/String.hpp"

#include "Loader/ShaderLoader.hpp"

namespace SD {

Scope<Device> Renderer::m_device;

Ref<UniformBuffer> Renderer::m_camera_UBO;
CameraData Renderer::m_camera_data;

Ref<VertexArray> Renderer::m_quad_vao;
Ref<VertexBuffer> Renderer::m_quad_vbo;
Ref<IndexBuffer> Renderer::m_quad_ibo;

Ref<VertexArray> Renderer::m_box_vao;
Ref<VertexBuffer> Renderer::m_box_vbo;
Ref<IndexBuffer> Renderer::m_box_ibo;

static std::stack<RenderPassInfo> s_render_pass_stacks;

void Renderer::SetRenderOperation(const RenderOperation& op) {
    op.depth_test ? m_device->Enable(Operation::DEPTH_TEST)
                  : m_device->Disable(Operation::DEPTH_TEST);
    m_device->SetDepthMask(op.depth_mask);

    m_device->SetDepthfunc(op.depth_func);
    op.blend ? m_device->Enable(Operation::BLEND)
             : m_device->Disable(Operation::BLEND);

    op.face_culling ? m_device->Enable(Operation::CULL_FACE)
                    : m_device->Disable(Operation::CULL_FACE);
    m_device->SetCullFace(op.cull_face);
}

void Renderer::BeginRenderPass(const RenderPassInfo& info) {
    s_render_pass_stacks.push(info);
    m_device->SetFramebuffer(info.framebuffer);
    m_device->SetViewport(0, 0, info.viewport_width, info.viewport_height);
    m_device->SetClearColor(info.clear_value[0], info.clear_value[1],
                            info.clear_value[2], info.clear_value[3]);
    if (info.clear_mask != BufferBitMask::NONE) {
        m_device->Clear(info.clear_mask);
    }
    SetRenderOperation(info.op);
}

void Renderer::BeginRenderSubpass(const RenderSubpassInfo& info) {
    auto& render_pass = GetCurrentRenderPass();
    m_device->SetFramebuffer(render_pass.framebuffer);
    m_device->SetViewport(0, 0, render_pass.viewport_width,
                          render_pass.viewport_height);

    m_device->DrawBuffers(render_pass.framebuffer, info.draw_buffer_count,
                          info.draw_buffer);
    SetRenderOperation(info.op);
}

void Renderer::EndRenderSubpass() { m_device->SetFramebuffer(nullptr); }

void Renderer::EndRenderPass() {
    m_device->SetFramebuffer(nullptr);
    s_render_pass_stacks.pop();
}

const RenderPassInfo& Renderer::GetCurrentRenderPass() {
    return s_render_pass_stacks.top();
}

glm::vec2 Renderer::GetCurrentBufferSize() {
    auto& render_pass = GetCurrentRenderPass();
    return {render_pass.framebuffer->GetWidth(),
            render_pass.framebuffer->GetHeight()};
}

bool Renderer::IsEmptyStack() { return s_render_pass_stacks.empty(); }

void Renderer::Init() {
    m_device = Device::Create();
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

void Renderer::DrawNDCQuad(const Shader& shader) {
    m_device->SetShader(&shader);
    Submit(*m_quad_vao, MeshTopology::TRIANGLES, m_quad_ibo->GetCount(), 0);
}

void Renderer::DrawNDCBox(const Shader& shader) {
    m_device->SetShader(&shader);
    Submit(*m_box_vao, MeshTopology::TRIANGLES, m_box_ibo->GetCount(), 0);
}

void Renderer::DrawToBuffer(int read_attachment, Framebuffer* draw_fb,
                            int draw_attachment, BufferBitMask mask) {
    Framebuffer* framebuffer = GetCurrentRenderPass().framebuffer;
    m_device->DrawBuffer(draw_fb, draw_attachment);
    m_device->ReadBuffer(framebuffer, read_attachment);
    m_device->BlitFramebuffer(framebuffer, 0, 0, framebuffer->GetWidth(),
                              framebuffer->GetHeight(), draw_fb, 0, 0,
                              framebuffer->GetWidth(), framebuffer->GetHeight(),
                              mask, BlitFilter::NEAREST);
}

void Renderer::DrawFromBuffer(int draw_attachment, Framebuffer* read_fb,
                              int read_attachment, BufferBitMask mask) {
    Framebuffer* framebuffer = GetCurrentRenderPass().framebuffer;
    m_device->DrawBuffer(framebuffer, draw_attachment);
    m_device->ReadBuffer(read_fb, read_attachment);
    m_device->BlitFramebuffer(read_fb, 0, 0, framebuffer->GetWidth(),
                              framebuffer->GetHeight(), framebuffer, 0, 0,
                              framebuffer->GetWidth(), framebuffer->GetHeight(),
                              mask, BlitFilter::NEAREST);
}

}  // namespace SD
