#include "System/ProfileSystem.hpp"
#include "Graphics/Device.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Core/Application.hpp"

namespace sd {

ProfileSystem::ProfileSystem(int width, int height)
    : m_camera(width, height, -1000.f, 1000.f), m_fps(20) {
    m_font = Asset::manager().load<Font>("fonts/opensans/OpenSans-Regular.ttf");
}

void ProfileSystem::onInit() {
    registerEvent(this, &ProfileSystem::onSizeEvent);
}

void ProfileSystem::onDestroy() { unregisterEvent<SizeEvent>(this); }

static float angle = 0.f;

void ProfileSystem::onTick(float dt) { angle += dt * 100; }

void ProfileSystem::onRender() {
    Renderer::setRenderTarget(Application::getRenderEngine().getRenderTarget());
    Device::instance().clear(BufferBitMask::DEPTH_BUFFER_BIT);
    Renderer2D::beginScene(m_camera);

    Renderer2D::setTextOrigin(
        -m_camera.getNearWidth() / 2.f,
        m_camera.getNearHeight() / 2.f - m_font->getPixelSize());
    std::wstring fpsStr =
        L"FPS:" + std::to_wstring(static_cast<uint32_t>(m_fps.getFps()));

    Renderer2D::drawTexture(
        Asset::manager().load<Texture>("textures/1_diagdown.png"),
        glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(1, 0, 0)));
    Renderer2D::drawText(*m_font, fpsStr);
    Renderer2D::endScene();
}

void ProfileSystem::onSizeEvent(const SizeEvent &event) {
    m_camera.resize(event.width, event.height);
}

}  // namespace sd
