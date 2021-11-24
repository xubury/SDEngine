#include "Renderer/System/ProfileSystem.hpp"
#include "Renderer/Renderer.hpp"
#include "Asset/Asset.hpp"
#include "Asset/FontLoader.hpp"

namespace SD {

ProfileSystem::ProfileSystem(RenderTarget *target, int width, int height)
    : System("Profile"),
      m_target(target),
      m_camera(CameraType::ORTHOGRAPHIC, glm::radians(45.f), width, height, 0.f,
               1000.f),
      m_fps(20) {}

void ProfileSystem::OnInit() {
    m_font = asset->LoadAndGet<Font>("fonts/wqy-microhei.ttc");
    m_font->LoadASCIIGlyph(20);
}

void ProfileSystem::OnPush() {
    dispatcher->Subscribe(this, &ProfileSystem::OnSizeEvent);
}

void ProfileSystem::OnPop() { dispatcher->Unsubscribe<WindowSizeEvent>(this); }

void ProfileSystem::OnTick(float) {}

void ProfileSystem::OnRender() {
    renderer->SetRenderTarget(*m_target);
    renderer->BeginScene(m_camera);
    std::string fpsStr =
        "FPS:" + std::to_string(static_cast<uint32_t>(m_fps.GetFPS()));

    uint8_t size = 20;
    renderer->SetTextOrigin(-m_camera.GetNearWidth() / 2.f,
                            m_camera.GetNearHeight() / 2.f - size);
    renderer->DrawText(*m_font, fpsStr, size, glm::mat4(1.0f));
    renderer->EndScene();
}

void ProfileSystem::OnSizeEvent(const WindowSizeEvent &event) {
    m_camera.Resize(event.width, event.height);
}

}  // namespace SD
