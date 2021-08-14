#include "Renderer/Renderer3D.hpp"
#include "Renderer/Renderer.hpp"
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
    Ref<Shader> shader;
};

static Renderer3DData s_data;

void Renderer3D::init() {
    SD_CORE_TRACE("Initializing Renderer3D...");
    s_data.cameraUBO = UniformBuffer::create(
        &s_data.cameraUBO, sizeof(CameraData), BufferIOType::STATIC);

    s_data.shader =
        Graphics::assetManager().load<Shader>("shaders/simple3d.glsl");
}

void Renderer3D::beginScene(const Camera &camera, const RenderTarget *target) {
    if (target) {
        target->use();
    } else {
        Renderer::getDefaultTarget()->use();
    }
    s_data.cameraBuffer.viewProjection = camera.getViewPorjection();
    s_data.cameraBuffer.viewPos = camera.getWorldPosition();
    s_data.cameraUBO->updateData(&s_data.cameraBuffer, sizeof(CameraData));
    s_data.shader->bind();
    s_data.shader->setUniformBuffer("Camera", *s_data.cameraUBO);
}

void Renderer3D::endScene() { s_data.shader->unbind(); }

void Renderer3D::drawMesh(const Mesh &mesh, const Transform &transform) {
    s_data.shader->setMat4("u_world", transform.getWorldTransform());
    Ref<VertexArray> vao = mesh.getVertexArray();
    Renderer::submit(*vao, MeshTopology::TRIANGLES,
                     vao->getIndexBuffer()->getCount(), 0);
}

}  // namespace sd
