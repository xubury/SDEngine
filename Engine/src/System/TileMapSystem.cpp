#include "System/TileMapSystem.hpp"
#include "Core/Input.hpp"

const int TILE_SIZE = 40;

namespace SD {

TileMapSystem::TileMapSystem() : System("TileMapSystem") {
    int outline_thickness = std::max<int>(0.05f * TILE_SIZE, 1);
    void *data = malloc(TILE_SIZE * TILE_SIZE * 4);
    memset(data, 0x77, TILE_SIZE * TILE_SIZE * 4);
    m_outline =
        Texture::Create(TILE_SIZE, TILE_SIZE,
                        TextureSpec(1, TextureType::TEX_2D, DataFormat::RGBA,
                                    DataFormatType::UBYTE));
    m_outline->SetPixels(0, 0, 0, TILE_SIZE, TILE_SIZE, 1, data);
    memset(data, 0x00, TILE_SIZE * TILE_SIZE * 4);
    m_outline->SetPixels(outline_thickness, outline_thickness, 0,
                         TILE_SIZE - outline_thickness * 2,
                         TILE_SIZE - outline_thickness * 2, 1, data);

    free(data);
}

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
        int size = m_map.GetTileSize();
        ImGui::TextUnformatted("Tile Size:");
        if (ImGui::InputInt("##Size", &size)) {
            m_map.SetTileSize(size);
        }

        auto texture = m_map.GetTexture();

        if (texture) {
            ImVec2 wsize = ImGui::GetContentRegionAvail();
            wsize.y = wsize.x * texture->GetHeight() / texture->GetWidth();
            float tile_size =
                m_map.GetTileSize() * wsize.x / texture->GetWidth();
            int cols = std::ceil(wsize.x / tile_size);
            int rows = std::ceil(wsize.y / tile_size);
            ImDrawList *DrawList = ImGui::GetWindowDrawList();
            ImGuiWindow *window = ImGui::GetCurrentWindow();

            ImRect bb(window->DC.CursorPos,
                      ImVec2(window->DC.CursorPos.x + cols * tile_size,
                             window->DC.CursorPos.y + rows * tile_size));
            ImGui::DrawTexture(*texture, wsize);
            for (int i = 0; i <= cols; ++i) {
                DrawList->AddLine(ImVec2(bb.Min.x + i * tile_size, bb.Min.y),
                                  ImVec2(bb.Min.x + i * tile_size, bb.Max.y),
                                  ImGui::GetColorU32(ImGuiCol_TextDisabled));
            }
            for (int i = 0; i <= rows; ++i) {
                DrawList->AddLine(ImVec2(bb.Min.x, bb.Min.y + i * tile_size),
                                  ImVec2(bb.Max.x, bb.Min.y + i * tile_size),
                                  ImGui::GetColorU32(ImGuiCol_TextDisabled));
            }
            if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0)) {
                auto [mouse_x, mouse_y] = ImGui::GetMousePos();
                auto [top, left] = bb.GetTL();
                glm::vec2 uv = glm::vec2(mouse_x - top, mouse_y - left);
                uv = uv / wsize.x * static_cast<float>(texture->GetWidth());
                m_tile.Set(m_map, uv);
            }
        }
    }
    ImGui::End();
}

void TileMapSystem::OnRender() {
    device->SetTarget(renderer->GetDefaultTarget());
    renderer->Begin(*scene->GetCamera());

    for (auto &[pos, tile] : m_contents) {
        renderer->DrawTexture(tile.GetTileMapTexture(), tile.GetTileMapUV(),
                              glm::vec3(MapTileToWorld(pos), -1.f),
                              glm::quat(1, 0, 0, 0),
                              glm::vec2(TILE_SIZE, TILE_SIZE));
    }
    const glm::ivec2 TILE_CNT(100, 100);
    renderer->DrawTexture(m_outline, {glm::vec2(0), TILE_CNT},
                          glm::vec3(-TILE_SIZE / 2.f, -TILE_SIZE / 2.f, 0),
                          glm::quat(1, 0, 0, 0), TILE_CNT * TILE_SIZE);
    renderer->End();
}

void TileMapSystem::Add(const glm::vec2 &world) {
    glm::vec2 tile_pos = MapWorldToTile(world);
    SD_TRACE("add tile at: {}, world:{}", tile_pos, world);
    if (m_tile.Valid()) {
        m_contents.emplace_back(tile_pos, m_tile);
    }
}

glm::ivec2 TileMapSystem::MapWorldToTile(const glm::vec2 &world) {
    glm::vec2 tile;
    tile.x = std::ceil(world.x / TILE_SIZE - 0.5f);
    tile.y = std::ceil(world.y / TILE_SIZE - 0.5f);
    return tile;
}

glm::vec2 TileMapSystem::MapTileToWorld(const glm::ivec2 &tile) {
    glm::vec2 world;
    world.x = tile.x * TILE_SIZE;
    world.y = tile.y * TILE_SIZE;
    return world;
}
}  // namespace SD
