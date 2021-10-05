#include "System/ProfileSystem.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Core/RenderEngine.hpp"

namespace sd {

ProfileSystem::ProfileSystem() : m_camera(800, 600, -1.f, 1.f) {}

void ProfileSystem::onInit() {
    registerEvent(this, &ProfileSystem::onSizeEvent);
}

void ProfileSystem::onTick(float) {}

void ProfileSystem::onRender() {
    Renderer2D::beginScene(m_camera);

    Renderer2D::setTextOrigin(-m_camera.getWidth() / 2.f,
                              m_camera.getHeight() / 2.f - 20);
    Renderer2D::drawText(L"FPS:" + std::to_wstring(m_fps.getFps()), 0.3);
    Renderer2D::endScene();
}

void ProfileSystem::onSizeEvent(const SizeEvent &event) {
    m_camera.resize(event.width, event.height);
}

}  // namespace sd
