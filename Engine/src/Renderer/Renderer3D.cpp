#include "Renderer/Renderer3D.hpp"
#include "Graphics/Buffer.hpp"
#include "Renderer/RenderTarget.hpp"
#include "Graphics/Camera.hpp"

namespace sd {

struct CameraData {
    glm::mat4 viewProjection;
    glm::vec3 viewPos;
};

struct Renderer3DData {
    CameraData cameraBuffer;
    Ref<UniformBuffer> cameraUBO;
};

static Renderer3DData s_data;

void Renderer3D::init() {
    SD_CORE_TRACE("Initializing Renderer3D...");
    s_data.cameraUBO = UniformBuffer::create(
        &s_data.cameraUBO, sizeof(CameraData), BufferIOType::STATIC);
}

void Renderer3D::beginScene(Camera &camera, const RenderTarget *target) {
    if (target) {
        target->use();
    } else {
        Renderer::getDefaultTarget().use();
    }
    s_data.cameraBuffer.viewProjection = camera.getViewPorjection();
    s_data.cameraBuffer.viewPos = camera.getWorldPosition();
    s_data.cameraUBO->updateData(&s_data.cameraBuffer, sizeof(CameraData));
}

void Renderer3D::endScene() {}

void Renderer3D::drawMesh(const Mesh &mesh) {
    Renderer::setWireframe(mesh.isWireframe());
    Ref<VertexArray> vao = mesh.getVertexArray();
    Renderer::submit(*vao, mesh.getTopology(),
                     vao->getIndexBuffer()->getCount(), 0);
}

void Renderer3D::setShader(Shader &shader) {
    shader.setUniformBuffer("Camera", *s_data.cameraUBO);
}

}  // namespace sd
