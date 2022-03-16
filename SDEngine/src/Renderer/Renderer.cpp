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
    SpriteRenderer::Init();
    MeshRenderer::Init();
}

void Renderer::Submit(const Shader& shader, const VertexArray& vao,
                      MeshTopology topology, size_t count, size_t offset,
                      bool index) {
    m_device->SetShader(&shader);
    vao.Bind();
    if (index) {
        m_device->DrawElements(topology, count, offset);
    } else {
        m_device->DrawArrays(topology, offset, count);
    }
}

}  // namespace SD
