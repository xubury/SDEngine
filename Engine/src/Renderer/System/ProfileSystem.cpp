#include "Renderer/System/ProfileSystem.hpp"
#include "Renderer/RenderEngine.hpp"
#include "Renderer/Renderer.hpp"
#include "Graphics/Device.hpp"

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
    Device::instance().setRenderTarget(RenderEngine::getRenderTarget());
    Renderer::beginScene(m_camera);
    std::wstring fpsStr =
        L"FPS:" + std::to_wstring(static_cast<uint32_t>(m_fps.getFps()));

    float size = 20;
    Renderer::setTextOrigin(-m_camera.getNearWidth() / 2.f,
                            m_camera.getNearHeight() / 2.f - size);
    Renderer::drawText(*m_font, fpsStr, size, glm::mat4(1.0f));
    Renderer::endScene();
}

void ProfileSystem::onSizeEvent(const SizeEvent &event) {
    m_camera.resize(event.width, event.height);
}

}  // namespace sd
