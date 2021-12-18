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

    m_fps.Probe();

    std::string fps_str =
        "FPS:" + std::to_string(static_cast<uint32_t>(m_fps.GetFPS()));
    std::string frame_time_str =
        "\nFrame Time:" +
        std::to_string(static_cast<uint32_t>(m_fps.GetFrameTime())) + "ms";

    renderer->GetDefaultTarget().Bind();
    Device::Instance().Clear(BufferBitMask::DEPTH_BUFFER_BIT);
    renderer->Begin(m_camera);

    renderer->SetTextOrigin(-m_camera.GetNearWidth() / 2.f,
                            m_camera.GetNearHeight() / 2.f - FONT_SIZE);
    renderer->DrawText(*m_font, fps_str, FONT_SIZE, glm::mat4(1.0f));
    renderer->DrawText(*m_font, frame_time_str, FONT_SIZE, glm::mat4(1.0f));
    renderer->DrawText(*m_font, "\n中文测试: 你好", FONT_SIZE, glm::mat4(1.0f));
    renderer->DrawText(*m_font, "\n中文測試: 你好", FONT_SIZE, glm::mat4(1.0f));
    renderer->DrawText(*m_font, "\n日本語テスト: こんにちは", FONT_SIZE,
                       glm::mat4(1.0f));
    // Primitive test
    // Line
    const int PRIMITIVE_SIZE = 15;
    renderer->DrawText(*m_font, "\nLine test: ", FONT_SIZE, glm::mat4(1.0f));
    glm::vec2 pos = renderer->GetTextCursor();
    pos.y += FONT_SIZE / 4.f;
    for (int i = 0; i < 10; ++i) {
        glm::vec4 color(0, 0, 0, (i + 1) / 10.f);
        color[i % 3] = 1.0f;
        renderer->DrawLine(glm::vec3(pos.x + PRIMITIVE_SIZE * i,
                                     pos.y + i % 2 * PRIMITIVE_SIZE, 0),
                           glm::vec3(pos.x + PRIMITIVE_SIZE * (i + 1),
                                     pos.y + (i + 1) % 2 * PRIMITIVE_SIZE, 0),
                           color);
    }
    // Quad
    renderer->DrawText(*m_font, "\nQuad test: ", FONT_SIZE, glm::mat4(1.0f));
    pos = renderer->GetTextCursor();
    pos.y += FONT_SIZE / 4.f;
    for (int i = 0; i < 10; ++i) {
        glm::vec4 color(0, 0, 0, (i + 1) / 10.f);
        color[i % 3] = 1.0f;
        renderer->DrawQuad(glm::vec3(pos.x, pos.y, 0), glm::quat(1, 0, 0, 0),
                           glm::vec3(PRIMITIVE_SIZE, PRIMITIVE_SIZE, 1), color);
        pos.x += PRIMITIVE_SIZE;
    }
    // Circle
    renderer->DrawText(*m_font, "\nCircle test: ", FONT_SIZE, glm::mat4(1.0f));
    pos = renderer->GetTextCursor();
    pos.y += FONT_SIZE / 4.f;
    for (int i = 0; i < 10; ++i) {
        glm::vec4 color(0, 0, 0, (i + 1) / 10.f);
        color[i % 3] = 1.0f;
        renderer->DrawCircle(glm::vec3(pos.x, pos.y, 0),
                             glm::vec2(PRIMITIVE_SIZE, PRIMITIVE_SIZE), color,
                             1.0f, 0.1f);
        pos.x += PRIMITIVE_SIZE;
    }
    renderer->End();
}

}  // namespace SD
