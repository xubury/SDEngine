#include "System/ProfileSystem.hpp"
#include "Renderer/Renderer.hpp"
#include "Asset/Asset.hpp"
#include "Core/Input.hpp"

namespace SD {

const uint8_t FONT_SIZE = 20;

const uint32_t FPS_CAPACITY = 50;

ProfileSystem::ProfileSystem(int width, int height)
    : System("ProfileSystem"),
      m_camera(CameraType::ORTHOGRAPHIC, glm::radians(45.f), width, height, 0.f,
               1000.f),
      m_fps(FPS_CAPACITY),
      m_is_show_message(true) {}

void ProfileSystem::OnInit() {
    m_font = asset->LoadAndGet<Font>("fonts/msyh.ttc");
    m_font->LoadGlyph(FONT_SIZE);
}

void ProfileSystem::OnPush() {
    m_size_handler =
        dispatcher->Register<WindowSizeEvent>([this](const WindowSizeEvent &e) {
            m_camera.Resize(e.width, e.height);
        });
}

void ProfileSystem::OnPop() { dispatcher->RemoveHandler(m_size_handler); }

void ProfileSystem::OnTick(float) {
    if (Input::IsKeyPressed(Keycode::F12)) {
        m_is_show_message = !m_is_show_message;
    }
}

void ProfileSystem::OnRender() {
    if (!m_is_show_message) return;

    renderer->SetRenderTarget(renderer->GetDefaultTarget());
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

}  // namespace SD
