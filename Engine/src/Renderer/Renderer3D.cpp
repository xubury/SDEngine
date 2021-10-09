#include "Renderer/Renderer3D.hpp"
#include "Graphics/RenderTarget.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Camera.hpp"

namespace sd {

void Renderer3D::init() { SD_CORE_TRACE("Initializing Renderer3D..."); }

void Renderer3D::beginScene(Camera &camera, Shader &shader) {
    Renderer::setCamera(camera, shader);
}

void Renderer3D::endScene() {}

void Renderer3D::drawMesh(const Mesh &mesh) {
    Device::instance().setPolygonMode(
        mesh.isWireframe() ? PolygonMode::LINE : PolygonMode::FILL, Face::BOTH);
    VertexArray *vao = mesh.getVertexArray();
    SD_CORE_ASSERT(vao, "Invalid mesh!");
    Renderer::submit(*vao, mesh.getTopology(),
                     vao->getIndexBuffer()->getCount(), 0);
}

}  // namespace sd
