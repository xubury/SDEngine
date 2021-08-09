#include "Renderer/Renderer.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Renderer/Renderer3D.hpp"
#include "Renderer/RenderTarget.hpp"
#include "Graphics/OpenGL/GLDevice.hpp"
#include "Graphics/Graphics.hpp"
#include "Utils/Log.hpp"

namespace sd {

Renderer &Renderer::instance() {
    static Renderer s_instance;
    return s_instance;
}

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
    instance().m_defaultTarget = createRef<RenderTarget>();
    instance().m_api = api;
}

API Renderer::getAPI() { return instance().m_api; }

void Renderer::submit(const VertexArray &vao, MeshTopology topology,
                      size_t count, size_t offset) {
    vao.bind();
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

RenderTarget &Renderer::getDefaultTarget() {
    return *instance().m_defaultTarget;
}

void Renderer::setDefaultTarget(Ref<RenderTarget> target) {
    instance().m_defaultTarget = target;
}

}  // namespace sd
