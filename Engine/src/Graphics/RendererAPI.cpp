#include "Graphics/RendererAPI.hpp"
#include "Graphics/OpenGL/GLDevice.hpp"
#include "Core/Log.hpp"

namespace sd {

static API s_api;

void RendererAPI::init(API api) {
    switch (api) {
        case API::OpenGL:
            Device::s_instance = createScope<GLDevice>();
            break;
        case API::None:
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    s_api = api;
}

API RendererAPI::getAPI() {
    return s_api;
}

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
