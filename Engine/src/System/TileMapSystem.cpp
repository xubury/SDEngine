#include "System/TileMapSystem.hpp"
#include "Core/Input.hpp"

const glm::ivec2 MAP_SIZE(16, 10);
const float GRID_SIZE = 40;

namespace SD {

TileMapSystem::TileMapSystem(int width, int height)
    : System("TileMapSystem"),
      m_tile_map(MAP_SIZE, GRID_SIZE),
      m_camera(CameraType::ORTHOGRAPHIC, glm::radians(45.f), width, height, 0.f,
               1000.f) {}

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
    device->SetTarget(renderer->GetDefaultTarget());
    device->Disable(Operation::DEPTH_TEST);
    renderer->Begin(m_camera);
    Ref<Texture> tex = m_tile_map.GetTexture();
    renderer->DrawTexture(tex, glm::vec3(0, 0, -1), glm::quat(1, 0, 0, 0),
                          glm::vec2(tex->GetWidth(), tex->GetHeight()));
    renderer->DrawTexture(m_tile_map.GetOutline(), {glm::vec2(0), MAP_SIZE},
                          glm::vec3(0), glm::quat(1, 0, 0, 0),
                          glm::vec2(tex->GetWidth(), tex->GetHeight()));
    renderer->DrawQuad(glm::vec3(0), glm::quat(1, 0, 0, 0), glm::vec2(10, 10),
                       glm::vec4(1, 0, 0, 1));
    renderer->End();
    device->Enable(Operation::DEPTH_TEST);
}

void TileMapSystem::SetCoordinate(const glm::vec2 &pos) {
    glm::vec2 world = m_camera.MapClipToWorld(pos);
    glm::ivec2 tile = m_tile_map.MapWorldToTile(world);
    if (m_tile_map.IsInBound(tile)) {
        m_tile_map.Set(tile, Tile());
    }
    m_active_tile = tile;
}

}  // namespace SD
