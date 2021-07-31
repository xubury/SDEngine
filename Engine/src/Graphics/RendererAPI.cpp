#include "Graphics/Device.hpp"
#include "Graphics/RendererAPI.hpp"

namespace sd {

namespace RendererAPI {

void drawElements(MeshTopology topology, size_t count, size_t offset) {
    Device::instance().drawElements(topology, count, offset);
}

void setClearColor(float r, float g, float b, float a) {
    Device::instance().setClearColor(r, g, b, a);
}

void clear() { Device::instance().clear(); }

void setViewport(int x, int y, int width, int height) {
    Device::instance().setViewport(x, y, width, height);
}

}  // namespace RendererAPI

}  // namespace sd
