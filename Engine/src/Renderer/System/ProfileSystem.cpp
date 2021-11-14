#include "Renderer/System/ProfileSystem.hpp"
#include "Renderer/Renderer.hpp"
#include "Asset/Asset.hpp"
#include "Asset/FontLoader.hpp"

namespace SD {

ProfileSystem::ProfileSystem(RenderTarget *target, int width, int height)
    : System("Profile"),
      m_target(target),
      m_camera(width, height, 0.f, 1000.f),
      m_fps(20) {
    auto resourceId = AssetManager::instance().loadAsset<Font>(
        "fonts/opensans/OpenSans-Regular.ttf");
    m_font = AssetManager::instance().get<Font>(resourceId);
}

void ProfileSystem::onInit() {
    registerEvent(this, &ProfileSystem::onSizeEvent);
}

void ProfileSystem::onDestroy() { unregisterEvent<Event::SizeEvent>(this); }

void ProfileSystem::onTick(float) {}

void ProfileSystem::onRender() {
    renderer->setRenderTarget(*m_target);
    renderer->beginScene(m_camera);
    std::wstring fpsStr =
        L"FPS:" + std::to_wstring(static_cast<uint32_t>(m_fps.getFps()));

    float size = 20;
    renderer->setTextOrigin(-m_camera.getNearWidth() / 2.f,
                            m_camera.getNearHeight() / 2.f - size);
    renderer->drawText(*m_font, fpsStr, size, glm::mat4(1.0f));
    renderer->endScene();
}

void ProfileSystem::onSizeEvent(const Event::SizeEvent &event) {
    m_camera.resize(event.width, event.height);
}

}  // namespace SD
