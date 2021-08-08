#include "Renderer/Renderer.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Renderer/Renderer3D.hpp"
#include "Graphics/OpenGL/GLDevice.hpp"
#include "Graphics/Graphics.hpp"
#include "Utils/Log.hpp"

namespace sd {

static API s_api;

void Renderer::init(API api) {
    SD_CORE_TRACE("Initializing Renderer...");
    switch (api) {
        case API::OpenGL:
            Device::s_instance = createScope<GLDevice>();
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    s_api = api;
}

API Renderer::getAPI() { return s_api; }

void Renderer::drawElements(MeshTopology topology, size_t count,
                            size_t offset) {
    Device::instance().drawElements(topology, count, offset);
}

void Renderer::setClearColor(float r, float g, float b, float a) {
    Device::instance().setClearColor(r, g, b, a);
}

void Renderer::clear() { Device::instance().clear(); }

void Renderer::setViewport(int x, int y, int width, int height) {
    Device::instance().setViewport(x, y, width, height);
}

void Renderer::setFramebuffer(const Framebuffer *framebuffer) {
    Device::instance().setFramebuffer(framebuffer);
}

}  // namespace sd
