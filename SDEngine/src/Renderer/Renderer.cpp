#include "Renderer/Renderer.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Renderer/Renderer3D.hpp"

namespace SD {

Device* Renderer::s_device;

Camera* Renderer::s_camera;
Ref<UniformBuffer> Renderer::s_camera_ubo;
CameraData Renderer::s_camera_data;

Ref<VertexArray> Renderer::s_quad_vao;
Ref<VertexBuffer> Renderer::s_quad_vbo;
Ref<IndexBuffer> Renderer::s_quad_ibo;

Ref<VertexArray> Renderer::s_box_vao;
Ref<VertexBuffer> Renderer::s_box_vbo;
Ref<IndexBuffer> Renderer::s_box_ibo;

bool Renderer::s_is_subpass_begin = false;

static std::stack<RenderPassInfo> s_render_pass_stacks;

void Renderer::SetRenderState(Framebuffer* framebuffer, int32_t viewport_width,
                              int32_t viewport_height,
                              const RenderOperation& op)
{
    s_device->SetFramebuffer(framebuffer);
    s_device->SetViewport(0, 0, viewport_width, viewport_height);
    op.depth_test ? s_device->Enable(Operation::DepthTest)
                  : s_device->Disable(Operation::DepthTest);
    s_device->SetDepthMask(op.depth_mask);

    s_device->SetDepthfunc(op.depth_func);
    op.blend ? s_device->Enable(Operation::Blend)
             : s_device->Disable(Operation::Blend);

    op.face_culling ? s_device->Enable(Operation::FaceCulling)
                    : s_device->Disable(Operation::FaceCulling);
    s_device->SetCullFace(op.cull_face);
}

void Renderer::BeginRenderPass(const RenderPassInfo& info)
{
    s_render_pass_stacks.push(info);
    info.framebuffer->Prepare();
    SetRenderState(info.framebuffer, info.viewport_width, info.viewport_height,
                   info.op);

    s_device->SetClearColor(info.clear_value[0], info.clear_value[1],
                            info.clear_value[2], info.clear_value[3]);
    if (info.clear_mask != BufferBitMask::None) {
        s_device->Clear(info.clear_mask);
    }
}

void Renderer::BeginRenderSubpass(const RenderSubpassInfo& info)
{
    SD_CORE_ASSERT(s_is_subpass_begin == false,
                   "Nested render subpass is not allowed!")
    s_is_subpass_begin = true;

    // restore render pass state
    auto& render_pass = GetCurrentRenderPass();
    SetRenderState(render_pass.framebuffer, render_pass.viewport_width,
                   render_pass.viewport_height, info.op);

    s_device->DrawBuffers(render_pass.framebuffer, info.draw_buffer_count,
                          info.draw_buffer);
}

void Renderer::EndRenderSubpass()
{
    s_device->SetFramebuffer(nullptr);
    s_device->SetShader(nullptr);
    s_is_subpass_begin = false;
}

void Renderer::EndRenderPass()
{
    s_render_pass_stacks.pop();
    s_device->SetFramebuffer(nullptr);
    s_device->SetShader(nullptr);
    // restore render pass state
    if (!IsEmptyStack()) {
        auto& render_pass = GetCurrentRenderPass();
        render_pass.framebuffer->Prepare();
        SetRenderState(render_pass.framebuffer, render_pass.viewport_width,
                       render_pass.viewport_height, render_pass.op);
    }
}

const RenderPassInfo& Renderer::GetCurrentRenderPass()
{
    return s_render_pass_stacks.top();
}

bool Renderer::IsEmptyStack() { return s_render_pass_stacks.empty(); }

void Renderer::Init(Device* device)
{
    s_device = device;
    SD_CORE_TRACE("Initializing Renderer");
    s_camera_ubo = UniformBuffer::Create(nullptr, sizeof(CameraData),
                                         BufferIOType::Dynamic);

    // NDC quad
    {
        const float quad_vert[] = {
            -1.0f, -1.0f, 0.f, 0.f,  0.f,   // bottom left
            1.0f,  -1.0f, 0.f, 1.0f, 0.f,   // bottom right
            1.0f,  1.0f,  0.f, 1.0f, 1.0f,  // top right
            -1.0f, 1.0f,  0.f, 0.f,  1.0f,  // top left
        };
        const uint32_t indices[] = {0, 1, 2, 2, 3, 0};
        s_quad_vbo = VertexBuffer::Create(quad_vert, sizeof(quad_vert),
                                          BufferIOType::Static);
        s_quad_ibo = IndexBuffer::Create(indices, 6, BufferIOType::Static);
        s_quad_vao = VertexArray::Create();
        VertexBufferLayout layout;
        layout.Push(BufferLayoutType::Float3);
        layout.Push(BufferLayoutType::Float2);
        s_quad_vao->AddBufferLayout(layout);
        s_quad_vao->BindVertexBuffer(*s_quad_vbo, 0);
        s_quad_vao->BindIndexBuffer(*s_quad_ibo);
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
        s_box_vao = VertexArray::Create();

        VertexBufferLayout layout;
        layout.Push(BufferLayoutType::Float3);
        s_box_vbo = VertexBuffer::Create(box_vert, sizeof(box_vert),
                                         BufferIOType::Static);
        s_box_ibo = IndexBuffer::Create(box_indices, 36, BufferIOType::Static);
        s_box_vao->AddBufferLayout(layout);
        s_box_vao->BindVertexBuffer(*s_box_vbo, 0);
        s_box_vao->BindIndexBuffer(*s_box_ibo);
    }
    Renderer2D::Init();
    Renderer3D::Init();
}

void Renderer::Submit(const VertexArray& vao, MeshTopology topology,
                      size_t count, size_t offset, bool index)
{
    s_device->SetVertexArray(&vao);
    if (index) {
        s_device->DrawElements(topology, count, offset);
    }
    else {
        s_device->DrawArrays(topology, offset, count);
    }
}

void Renderer::SetCamera(Camera& camera)
{
    s_camera = &camera;
    s_camera_data.view = camera.GetView();
    s_camera_data.projection = camera.GetProjection();
    s_camera_ubo->UpdateData(&s_camera_data, sizeof(CameraData));
}

Camera* Renderer::GetCamera() { return s_camera; }

void Renderer::BindCamera(Shader& shader)
{
    shader.SetUniformBuffer("Camera", *s_camera_ubo);
}

void Renderer::ComputeImage(Shader& shader, int32_t num_group_x,
                            int32_t num_group_y, int32_t num_group_z)
{
    s_device->SetShader(&shader);
    s_device->DispatchCompute(num_group_x, num_group_y, num_group_z);
    s_device->MemoryBarrier(BarrierBit::ImageAccess);
}

void Renderer::DrawNDCQuad(const Shader& shader)
{
    s_device->SetShader(&shader);
    Submit(*s_quad_vao, MeshTopology::Triangles, s_quad_ibo->GetCount(), 0);
}

void Renderer::DrawNDCBox(const Shader& shader)
{
    s_device->SetShader(&shader);
    Submit(*s_box_vao, MeshTopology::Triangles, s_box_ibo->GetCount(), 0);
}

void Renderer::BlitToBuffer(int read_attachment, Framebuffer* draw_fb,
                            int draw_attachment, BufferBitMask mask)
{
    auto& render_pass = GetCurrentRenderPass();
    Vector2i size(render_pass.viewport_width, render_pass.viewport_height);
    Framebuffer* framebuffer = render_pass.framebuffer;
    s_device->DrawBuffer(draw_fb, draw_attachment);
    s_device->ReadBuffer(framebuffer, read_attachment);
    s_device->BlitFramebuffer(framebuffer, 0, 0, size.x, size.y, draw_fb, 0, 0,
                              size.x, size.y, mask, BlitFilter::Nearest);
}

void Renderer::BlitFromBuffer(int draw_attachment, Framebuffer* read_fb,
                              int read_attachment, BufferBitMask mask)
{
    auto& render_pass = GetCurrentRenderPass();
    Framebuffer* framebuffer = render_pass.framebuffer;
    Vector2i size(render_pass.viewport_width, render_pass.viewport_height);
    s_device->DrawBuffer(framebuffer, draw_attachment);
    s_device->ReadBuffer(read_fb, read_attachment);
    s_device->BlitFramebuffer(read_fb, 0, 0, size.x, size.y, framebuffer, 0, 0,
                              size.x, size.y, mask, BlitFilter::Nearest);
}

}  // namespace SD
