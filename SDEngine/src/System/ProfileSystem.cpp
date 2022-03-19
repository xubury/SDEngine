#include "System/ProfileSystem.hpp"
#include "Renderer/SpriteRenderer.hpp"
#include "Core/Input.hpp"
#include "Loader/FontLoader.hpp"

namespace SD {

const uint32_t FPS_CAPACITY = 50;

ProfileSystem::ProfileSystem()
    : System("ProfileSystem"),
      m_camera(CameraType::Orthographic, glm::radians(45.f), 100, 100, 0.f,
               1000.f),
      m_fps(FPS_CAPACITY),
      m_is_show_message(true) {}

void ProfileSystem::OnInit() {
    System::OnInit();
    m_font = FontLoader::LoadFont("assets/fonts/msyh.ttc", 20, true);
}

void ProfileSystem::OnPush() {
    m_size_handler = EventSystem::Get().Register<ViewportSizeEvent>(
        [this](const ViewportSizeEvent &e) {
            m_camera.Resize(e.width, e.height);
        });
}

void ProfileSystem::OnPop() {
    EventSystem::Get().RemoveHandler(m_size_handler);
}

void ProfileSystem::OnTick(float) {
    if (Input::IsKeyPressed(Keycode::F12)) {
        m_is_show_message = !m_is_show_message;
    }
}

void ProfileSystem::OnRender() {
    if (!m_is_show_message) return;

    m_fps.Probe();

    std::string fps_str;
    fps_str.resize(255);
    snprintf(fps_str.data(), fps_str.size(), "FPS: %.2f(%.2f ms)",
             m_fps.GetFPS(), m_fps.GetFrameTime());
    // only draw colors
    const int index = 0;
    Renderer::BeginRenderSubpass(RenderSubpassInfo{&index, 1});
    glm::vec2 size = Renderer::GetCurrentBufferSize();
    m_camera.Resize(size.x, size.y);
    SpriteRenderer::Begin(m_camera);
    SpriteRenderer::SetTextOrigin(
        -m_camera.GetNearWidth() / 2,
        m_camera.GetNearHeight() / 2 - m_font->GetPixelHeight());

    SpriteRenderer::DrawText(*m_font, fps_str, glm::mat4(1.0f));
    SpriteRenderer::DrawText(*m_font, "\n中文测试: 你好", glm::mat4(1.0f));
    SpriteRenderer::DrawText(*m_font, "\n中文測試: 你好", glm::mat4(1.0f));
    SpriteRenderer::DrawText(*m_font, "\n日本語テスト: こんにちは",
                             glm::mat4(1.0f));
    // Primitive test
    // Line
    const int PRIMITIVE_SIZE = 15;
    SpriteRenderer::DrawText(*m_font, "\nLine test: ", glm::mat4(1.0f));
    glm::vec2 pos = SpriteRenderer::GetTextCursor();
    pos.y += m_font->GetPixelHeight() / 4.f;
    for (int i = 0; i < 10; ++i) {
        glm::vec4 color(0, 0, 0, (i + 1) / 10.f);
        color[i % 3] = 1.0f;
        SpriteRenderer::DrawLine(
            glm::vec3(pos.x + PRIMITIVE_SIZE * i,
                      pos.y + i % 2 * PRIMITIVE_SIZE, 0),
            glm::vec3(pos.x + PRIMITIVE_SIZE * (i + 1),
                      pos.y + (i + 1) % 2 * PRIMITIVE_SIZE, 0),
            color);
    }
    // Quad
    SpriteRenderer::DrawText(*m_font, "\nQuad test: ", glm::mat4(1.0f));
    pos = SpriteRenderer::GetTextCursor();
    pos.y += m_font->GetPixelHeight() / 4.f;
    for (int i = 0; i < 10; ++i) {
        glm::vec4 color(0, 0, 0, (i + 1) / 10.f);
        color[i % 3] = 1.0f;
        SpriteRenderer::DrawQuad(
            glm::vec3(pos.x, pos.y, 0), glm::quat(1, 0, 0, 0),
            glm::vec3(PRIMITIVE_SIZE, PRIMITIVE_SIZE, 1), color);
        pos.x += PRIMITIVE_SIZE;
    }
    // Circle
    SpriteRenderer::DrawText(*m_font, "\nCircle test: ", glm::mat4(1.0f));
    pos = SpriteRenderer::GetTextCursor();
    pos.y += m_font->GetPixelHeight() / 4.f;
    for (int i = 0; i < 10; ++i) {
        glm::vec4 color(0, 0, 0, (i + 1) / 10.f);
        color[i % 3] = 1.0f;
        SpriteRenderer::DrawCircle(glm::vec3(pos.x, pos.y, 0),
                                   glm::vec2(PRIMITIVE_SIZE, PRIMITIVE_SIZE),
                                   color, 1.0f, 0.1f);
        pos.x += PRIMITIVE_SIZE;
    }
    SpriteRenderer::End();
    Renderer::EndRenderSubpass();
}

}  // namespace SD
