#include "ProfileSystem.hpp"
#include "Renderer/Event.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Core/Input.hpp"
#include "Loader/FontLoader.hpp"

namespace SD {

const uint32_t FPS_CAPACITY = 50;

ProfileSystem::ProfileSystem()
    : ECSSystem("ProfileSystem"),
      m_camera(CameraType::Orthographic, glm::radians(45.f), 100, 100, 0.f,
               1000.f),
      m_fps(FPS_CAPACITY),
      m_is_show_message(true)
{
}

void ProfileSystem::OnInit()
{
    ECSSystem::OnInit();
    m_font = FontLoader::LoadFont("assets/fonts/msyh.ttc", 20);
}

void ProfileSystem::OnPush()
{
    m_size_handler = GetEventDispatcher().Register<RenderSizeEvent>(
        [this](const RenderSizeEvent &e) {
            m_camera.Resize(e.width, e.height);
        });
}

void ProfileSystem::OnPop()
{
    GetEventDispatcher().RemoveHandler(m_size_handler);
}

void ProfileSystem::OnTick(float)
{
    if (Input::IsKeyPressed(Keycode::F12)) {
        m_is_show_message = !m_is_show_message;
    }
}

void ProfileSystem::OnRender()
{
    if (!m_is_show_message) return;

    m_fps.Probe();

    char fps_str[255];
    snprintf(fps_str, sizeof(fps_str), "FPS: %.2f(%.2f ms)", m_fps.GetFPS(),
             m_fps.GetFrameTime());
    // only draw colors
    const int index = 0;
    Renderer::BeginRenderSubpass(RenderSubpassInfo{&index, 1});
    Renderer2D::Begin(m_camera);
    Renderer2D::SetTextOrigin(
        -m_camera.GetNearWidth() / 2,
        m_camera.GetNearHeight() / 2 - m_font->GetPixelHeight());

    Renderer2D::DrawText(*m_font, fps_str, Matrix4f(1.0f));
    Renderer2D::DrawText(*m_font, "\n中文测试: 你好", Matrix4f(1.0f));
    Renderer2D::DrawText(*m_font, "\n中文測試: 你好", Matrix4f(1.0f));
    Renderer2D::DrawText(*m_font, "\n日本語テスト: こんにちは", Matrix4f(1.0f));
    // Primitive test
    // Line
    const int PRIMITIVE_SIZE = 15;
    Renderer2D::DrawText(*m_font, "\nLine test: ", Matrix4f(1.0f));
    Vector2f pos = Renderer2D::GetTextCursor();
    pos.y += m_font->GetPixelHeight() / 4.f;
    for (int i = 0; i < 10; ++i) {
        Vector4f color(0, 0, 0, (i + 1) / 10.f);
        color[i % 3] = 1.0f;
        Renderer2D::DrawLine(Vector3f(pos.x + PRIMITIVE_SIZE * i,
                                      pos.y + i % 2 * PRIMITIVE_SIZE, 0),
                             Vector3f(pos.x + PRIMITIVE_SIZE * (i + 1),
                                      pos.y + (i + 1) % 2 * PRIMITIVE_SIZE, 0),
                             color);
    }
    // Quad
    Renderer2D::DrawText(*m_font, "\nQuad test: ", Matrix4f(1.0f));
    pos = Renderer2D::GetTextCursor();
    pos.y += m_font->GetPixelHeight() / 4.f;
    for (int i = 0; i < 10; ++i) {
        Vector4f color(0, 0, 0, (i + 1) / 10.f);
        color[i % 3] = 1.0f;
        Renderer2D::DrawQuad(Vector3f(pos.x, pos.y, 0), Quaternion(1, 0, 0, 0),
                             Vector3f(PRIMITIVE_SIZE, PRIMITIVE_SIZE, 1),
                             color);
        pos.x += PRIMITIVE_SIZE;
    }
    // Circle
    Renderer2D::DrawText(*m_font, "\nCircle test: ", Matrix4f(1.0f));
    pos = Renderer2D::GetTextCursor();
    pos.y += m_font->GetPixelHeight() / 4.f;
    for (int i = 0; i < 10; ++i) {
        Vector4f color(0, 0, 0, (i + 1) / 10.f);
        color[i % 3] = 1.0f;
        Renderer2D::DrawCircle(Vector3f(pos.x, pos.y, 0),
                               Vector2f(PRIMITIVE_SIZE, PRIMITIVE_SIZE), color,
                               1.0f, 0.1f);
        pos.x += PRIMITIVE_SIZE;
    }
    Renderer2D::End();
    Renderer::EndRenderSubpass();
}

}  // namespace SD
