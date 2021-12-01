#include "System/ProfileSystem.hpp"
#include "Renderer/Renderer.hpp"
#include "Asset/Asset.hpp"

namespace SD {

const uint8_t FONT_SIZE = 20;

const uint32_t FPS_CAPACITY = 50;

ProfileSystem::ProfileSystem(RenderTarget *target, int width, int height)
    : System("Profile"),
      m_target(target),
      m_camera(CameraType::ORTHOGRAPHIC, glm::radians(45.f), width, height, 0.f,
               1000.f),
      m_fps(FPS_CAPACITY) {}

void ProfileSystem::OnInit() {
    m_font = asset->LoadAndGet<Font>("fonts/msyh.ttc");
    m_font->LoadGlyph(FONT_SIZE);
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

    renderer->SetTextOrigin(-m_camera.GetNearWidth() / 2.f,
                            m_camera.GetNearHeight() / 2.f - FONT_SIZE);
    renderer->DrawText(*m_font, fpsStr, FONT_SIZE, glm::mat4(1.0f));
    renderer->DrawText(*m_font, "\n中文测试: 你好", FONT_SIZE, glm::mat4(1.0f));
    renderer->DrawText(*m_font, "\n中文測試: 你好", FONT_SIZE, glm::mat4(1.0f));
    renderer->DrawText(*m_font, "\n日本語テスト: こんにちは", FONT_SIZE,
                       glm::mat4(1.0f));
    renderer->EndScene();
}

void ProfileSystem::OnSizeEvent(const WindowSizeEvent &event) {
    m_camera.Resize(event.width, event.height);
}

}  // namespace SD
