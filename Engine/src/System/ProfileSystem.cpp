#include "System/ProfileSystem.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Renderer.hpp"
#include "Core/Application.hpp"

namespace sd {

ProfileSystem::ProfileSystem(int width, int height)
    : m_camera(width, height, 0.f, 1000.f), m_fps(20) {
    m_font = Asset::manager().load<Font>("fonts/opensans/OpenSans-Regular.ttf");
}

void ProfileSystem::onInit() {
    registerEvent(this, &ProfileSystem::onSizeEvent);
}

void ProfileSystem::onDestroy() { unregisterEvent<SizeEvent>(this); }

void ProfileSystem::onTick(float) {}

void ProfileSystem::onRender() {
    Device::instance().disable(Operation::DEPTH_TEST);

    Renderer::setRenderTarget(Application::getRenderEngine().getRenderTarget());
    Renderer::beginScene(m_camera);

    std::wstring fpsStr =
        L"FPS:" + std::to_wstring(static_cast<uint32_t>(m_fps.getFps()));

    float size = 20;
    Renderer::setTextOrigin(-m_camera.getNearWidth() / 2.f,
                            m_camera.getNearHeight() / 2.f - size);
    glm::mat4 t = glm::scale(glm::mat4(1.0f), glm::vec3(size, size, 1));
    Renderer::drawText(*m_font, fpsStr, t, glm::vec4(1.0f));
    Renderer::endScene();
    Device::instance().enable(Operation::DEPTH_TEST);
}

void ProfileSystem::onSizeEvent(const SizeEvent &event) {
    m_camera.resize(event.width, event.height);
}

}  // namespace sd
