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

void Renderer::Init(Device *device) {
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
