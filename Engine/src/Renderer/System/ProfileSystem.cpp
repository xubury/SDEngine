#include "Renderer/System/ProfileSystem.hpp"
#include "Renderer/Renderer.hpp"
#include "Asset/Asset.hpp"
#include "Asset/FontLoader.hpp"

namespace SD {

ProfileSystem::ProfileSystem(RenderTarget *target, int width, int height)
    : System("Profile"),
      m_target(target),
      m_camera(width, height, 0.f, 1000.f),
      m_fps(20) {}

void ProfileSystem::OnPush() {
    auto resourceId =
        asset->loadAsset<Font>("fonts/opensans/OpenSans-Regular.ttf");
    m_font = asset->Get<Font>(resourceId);
    dispatcher->Subscribe(this, &ProfileSystem::OnSizeEvent);
}

void ProfileSystem::OnPop() { dispatcher->Unsubscribe<WindowSizeEvent>(this); }

void ProfileSystem::OnTick(float) {}

void ProfileSystem::OnRender() {
    renderer->SetRenderTarget(*m_target);
    renderer->BeginScene(m_camera);
    std::wstring fpsStr =
        L"FPS:" + std::to_wstring(static_cast<uint32_t>(m_fps.GetFps()));

    float size = 20;
    renderer->SetTextOrigin(-m_camera.GetNearWidth() / 2.f,
                            m_camera.GetNearHeight() / 2.f - size);
    renderer->DrawText(*m_font, fpsStr, size, glm::mat4(1.0f));
    renderer->EndScene();
}

void ProfileSystem::OnSizeEvent(const WindowSizeEvent &event) {
    m_camera.Resize(event.width, event.height);
}

}  // namespace SD
