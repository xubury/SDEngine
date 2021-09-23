#include "Renderer/Renderer.hpp"
#include "Renderer/RenderTarget.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Camera.hpp"
#include "Utils/Log.hpp"

namespace sd {

struct CameraData {
    glm::mat4 viewProjection;
    glm::vec3 viewPos;
};

struct RendererData {
    CameraData cameraData;
    Ref<UniformBuffer> cameraUBO;
};

static RendererData s_data;

Renderer &Renderer::instance() {
    static Renderer s_instance;
    return s_instance;
}

void Renderer::init() {
    SD_CORE_TRACE("Initializing Renderer...");
    s_data.cameraUBO = UniformBuffer::create(
        &s_data.cameraData, sizeof(CameraData), BufferIOType::STATIC);
}

void Renderer::submit(const VertexArray &vao, MeshTopology topology,
                      size_t count, size_t offset) {
    vao.bind();
    Device::instance().drawElements(topology, count, offset);
}

RenderTarget &Renderer::getDefaultTarget() {
    return instance().m_defaultTarget;
}

void Renderer::setDefaultTarget(const RenderTarget &target) {
    instance().m_defaultTarget = target;
}

void Renderer::setShader(Shader &shader) {
    shader.bind();
    shader.setUniformBuffer("Camera", *s_data.cameraUBO);
}

void Renderer::setCamera(Camera &camera) {
    s_data.cameraData.viewProjection = camera.getViewPorjection();
    s_data.cameraData.viewPos = camera.getWorldPosition();
    s_data.cameraUBO->updateData(&s_data.cameraData, sizeof(CameraData));
}

}  // namespace sd
