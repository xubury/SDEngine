#include "Graphics/Device.hpp"
#include "Graphics/RendererAPI.hpp"

namespace sd {

void RendererAPI::drawElements(MeshTopology topology, size_t count,
                               size_t offset) {
    Device::instance().drawElements(topology, count, offset);
}

void RendererAPI::setClearColor(float r, float g, float b, float a) {
    Device::instance().setClearColor(r, g, b, a);
}

void RendererAPI::clear() { Device::instance().clear(); }

void RendererAPI::setViewport(int x, int y, int width, int height) {
    Device::instance().setViewport(x, y, width, height);
}

}  // namespace sd
