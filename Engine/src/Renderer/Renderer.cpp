#include "Renderer/Renderer.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Device.hpp"
#include "Utility/Log.hpp"

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

void Renderer::setRenderTarget(const RenderTarget &target) {
    Device::instance().setFramebuffer(target.getFramebuffer());
    Device::instance().setViewport(target.getX(), target.getY(),
                                   target.getWidth(), target.getHeight());
}

void Renderer::setCamera(Camera &camera, Shader &shader) {
    s_data.cameraData.viewProjection = camera.getViewPorjection();
    s_data.cameraData.viewPos = camera.getWorldPosition();
    s_data.cameraUBO->updateData(&s_data.cameraData, sizeof(CameraData));
    shader.setUniformBuffer("Camera", *s_data.cameraUBO);
}

}  // namespace sd
