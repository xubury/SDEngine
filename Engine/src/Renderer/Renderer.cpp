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

void Renderer::init() {
    SD_CORE_TRACE("Initializing Renderer...");
    switch (Graphics::getAPI()) {
        case API::OpenGL:
            instance().m_device = createScope<GLDevice>();
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
}

void Renderer::submit(const VertexArray &vao, MeshTopology topology,
                      size_t count, size_t offset) {
    vao.bind();
    instance().m_device->drawElements(topology, count, offset);
}

void Renderer::setClearColor(float r, float g, float b, float a) {
    instance().m_device->setClearColor(r, g, b, a);
}

void Renderer::clear() { instance().m_device->clear(); }

void Renderer::setViewport(int x, int y, int width, int height) {
    instance().m_device->setViewport(x, y, width, height);
}

void Renderer::setFramebuffer(const Framebuffer *framebuffer) {
    instance().m_device->setFramebuffer(framebuffer);
}

void Renderer::setWireframe(bool wireframe) {
    instance().m_device->setWireframe(wireframe);
}

void Renderer::setDepthMask(bool depthMask) {
    instance().m_device->setDepthMask(depthMask);
}

void Renderer::setBlend(bool blend) {
    instance().m_device->setBlend(blend);
}

RenderTarget &Renderer::getDefaultTarget() {
    return instance().m_defaultTarget;
}

void Renderer::setDefaultTarget(const RenderTarget &target) {
    instance().m_defaultTarget = target;
}

}  // namespace sd
