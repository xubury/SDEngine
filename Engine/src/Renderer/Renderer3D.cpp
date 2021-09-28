#include "Renderer/Renderer3D.hpp"
#include "Renderer/RenderTarget.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Camera.hpp"

namespace sd {

void Renderer3D::init() { SD_CORE_TRACE("Initializing Renderer3D..."); }

void Renderer3D::beginScene(Camera &camera) {
    Renderer::setCamera(camera);
}

void Renderer3D::endScene() {}

void Renderer3D::drawMesh(const Mesh &mesh) {
    Device::instance().setWireframe(mesh.isWireframe());
    VertexArray *vao = mesh.getVertexArray();
    if (vao) {
        Renderer::submit(*vao, mesh.getTopology(),
                         vao->getIndexBuffer()->getCount(), 0);
    }
}

}  // namespace sd
