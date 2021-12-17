#include "System/TileMapSystem.hpp"
#include "Core/Input.hpp"

const glm::ivec2 MAP_SIZE(640, 480);
const glm::ivec2 GRID_SIZE(40, 40);

namespace SD {

TileMapSystem::TileMapSystem(int width, int height)
    : System("TileMapSystem"),
      m_tile_map(MAP_SIZE, GRID_SIZE),
      m_camera(CameraType::ORTHOGRAPHIC, glm::radians(45.f), width, height, 0.f,
               1000.f) {
    m_tile_map.Set(glm::ivec2(3, 2), Tile());
}

void TileMapSystem::OnTick(float) {
    if (Input::IsKeyDown(Keycode::W)) {
        m_camera.TranslateWorld(m_camera.GetWorldUp());
    }
    if (Input::IsKeyDown(Keycode::S)) {
        m_camera.TranslateWorld(-m_camera.GetWorldUp());
    }
    if (Input::IsKeyDown(Keycode::A)) {
        m_camera.TranslateWorld(-m_camera.GetWorldRight());
    }
    if (Input::IsKeyDown(Keycode::D)) {
        m_camera.TranslateWorld(m_camera.GetWorldRight());
    }
}

void TileMapSystem::OnPush() {
    m_size_handler =
        dispatcher->Register<WindowSizeEvent>([this](const WindowSizeEvent &e) {
            m_camera.Resize(e.width, e.height);
        });
}

void TileMapSystem::OnPop() { dispatcher->RemoveHandler(m_size_handler); }

void TileMapSystem::OnRender() {
    Device::instance().Disable(Operation::DEPTH_TEST);
    renderer->GetDefaultTarget().Bind();
    renderer->Begin(m_camera);
    Ref<Texture> tex = m_tile_map.GetTexture();
    renderer->DrawTexture(tex, glm::vec3(0), glm::quat(1, 0, 0, 0),
                          glm::vec2(tex->GetWidth(), tex->GetHeight()));
    renderer->DrawTexture(m_tile_map.GetOutline(),
                          {glm::vec2{0, MAP_SIZE.y / GRID_SIZE.y},
                           glm::vec2(MAP_SIZE.x / GRID_SIZE.x, 0)},
                          glm::vec3(0), glm::quat(1, 0, 0, 0),
                          glm::vec2(tex->GetWidth(), tex->GetHeight()));
    renderer->End();
    Device::instance().Enable(Operation::DEPTH_TEST);
}

}  // namespace SD
