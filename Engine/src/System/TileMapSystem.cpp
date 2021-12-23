#include "System/TileMapSystem.hpp"
#include "Core/Input.hpp"

namespace SD {

TileMapSystem::TileMapSystem() : System("TileMapSystem") {}

void TileMapSystem::OnTick(float) {}

void TileMapSystem::OnPush() {}

void TileMapSystem::OnPop() {}

void TileMapSystem::OnImGui() {
    ImGui::Begin("TileMap System");
    {
        std::string path = m_fileDialogInfo.result_path.string();
        ImGui::InputText("##Path", path.data(), path.size(),
                         ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        if (ImGui::Button("Open")) {
            m_fileDialogOpen = true;
            m_fileDialogInfo.type = ImGuiFileDialogType::OPEN_FILE;
            m_fileDialogInfo.title = "Open File";
            m_fileDialogInfo.file_name = "";
            m_fileDialogInfo.directory_path = asset->GetRootPath();
        }
        if (ImGui::FileDialog(&m_fileDialogOpen, &m_fileDialogInfo)) {
            auto bmp = asset->LoadAndGet<Bitmap>(m_fileDialogInfo.result_path);
            m_map.SetTileMap(*bmp);
        }
        glm::ivec2 size = m_map.GetTileSize();
        ImGui::TextUnformatted("Tile Size:");
        if (ImGui::InputInt2("##Size", &size.x)) {
            m_map.SetTileSize(size);
            m_has_select = false;
        }

        auto texture = m_map.GetTexture();

        if (texture) {
            ImVec2 wsize = ImGui::GetContentRegionAvail();
            float aspect = wsize.x / texture->GetWidth();
            wsize.y = texture->GetHeight() * aspect;
            glm::vec2 tile_size = glm::vec2(m_map.GetTileSize()) * aspect;

            int cols = std::ceil(wsize.x / tile_size.x);
            int rows = std::ceil(wsize.y / tile_size.y);
            ImDrawList *DrawList = ImGui::GetWindowDrawList();
            ImGuiWindow *window = ImGui::GetCurrentWindow();

            ImRect bb(window->DC.CursorPos,
                      ImVec2(window->DC.CursorPos.x + cols * tile_size.x,
                             window->DC.CursorPos.y + rows * tile_size.y));
            ImGui::DrawTexture(*texture, wsize);
            for (int i = 0; i <= cols; ++i) {
                DrawList->AddLine(ImVec2(bb.Min.x + i * tile_size.x, bb.Min.y),
                                  ImVec2(bb.Min.x + i * tile_size.x, bb.Max.y),
                                  ImGui::GetColorU32(ImGuiCol_TextDisabled));
            }
            for (int i = 0; i <= rows; ++i) {
                DrawList->AddLine(ImVec2(bb.Min.x, bb.Min.y + i * tile_size.y),
                                  ImVec2(bb.Max.x, bb.Min.y + i * tile_size.y),
                                  ImGui::GetColorU32(ImGuiCol_TextDisabled));
            }
            if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0)) {
                auto [mouse_x, mouse_y] = ImGui::GetMousePos();
                auto [top, left] = bb.GetTL();
                auto image_pos =
                    glm::vec2(mouse_x - top, mouse_y - left) / aspect;
                m_selected_tile =
                    Tile(m_map.GetTexture(), m_map.GetTileUVs(image_pos));
                m_has_select = true;
            }
            if (m_has_select) {
                const ImU32 COLOR = 0xff00ff00;
                const float THICKNESS = 2.f;
                auto [left, top] = bb.GetTL();
                auto uvs = m_selected_tile.GetUVs();
                ImRect active_grid(
                    ImVec2(left + uvs[0].x * wsize.x, top + uvs[0].y * wsize.y),
                    ImVec2(left + uvs[1].x * wsize.x,
                           top + uvs[1].y * wsize.y));
                DrawList->AddQuad(active_grid.GetTL(), active_grid.GetTR(),
                                  active_grid.GetBR(), active_grid.GetBL(),
                                  COLOR, THICKNESS);
            }
        }
    }
    ImGui::End();
}

void TileMapSystem::OnRender() {
    const glm::ivec2 TILE_SIZE = m_layout.GetTileSize();
    device->SetTarget(renderer->GetDefaultTarget());

    renderer->Begin(*scene->GetCamera());
    if (m_has_select) {
        renderer->DrawTexture(
            m_selected_tile.GetTexture(), m_selected_tile.GetUVs(),
            glm::vec3(m_layout.MapTileToWorld(m_active_tile_pos), -1.f),
            glm::quat(1, 0, 0, 0), TILE_SIZE);
    }
    for (auto &[pos, tile] : m_layout.GetTiles()) {
        renderer->DrawTexture(tile.GetTexture(), tile.GetUVs(),
                              glm::vec3(m_layout.MapTileToWorld(pos), -1.f),
                              glm::quat(1, 0, 0, 0), TILE_SIZE);
    }
    const glm::ivec2 TILE_CNT(100, 100);
    renderer->DrawTexture(m_layout.GetGridTexture(), {glm::vec2(0), TILE_CNT},
                          glm::vec3(-TILE_SIZE.x / 2.f, -TILE_SIZE.y / 2.f, 0),
                          glm::quat(1, 0, 0, 0), TILE_CNT * TILE_SIZE);
    renderer->End();
}

void TileMapSystem::SetActivePos(const glm::vec2 &world) {
    m_active_tile_pos = m_layout.MapWorldToTile(world);
}

void TileMapSystem::AddSelectTileToWorld() {
    if (m_has_select) {
        SD_TRACE("add tile at: {}", m_active_tile_pos);
        m_layout.Set(m_active_tile_pos, m_selected_tile);
    }
}

}  // namespace SD
