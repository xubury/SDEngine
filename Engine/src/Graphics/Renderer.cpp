#include "Graphics/Renderer.hpp"
#include "Graphics/Renderer2D.hpp"
#include "Graphics/OpenGL/GLDevice.hpp"
#include "Graphics/GraphicsManager.hpp"
#include "Utils/Log.hpp"

namespace sd {

static API s_api;

void Renderer::init(API api) {
    GraphicsManager::shaders().setRootPath("assets/shaders/");
    GraphicsManager::shaders().setLoader<ShaderLoader>();

    GraphicsManager::textures().setRootPath("assets/textures/");
    GraphicsManager::textures().setLoader<TextureLoader>();

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

    Renderer2D::init();
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
