#include "System/TileMapSystem.hpp"
#include "Core/Input.hpp"

namespace SD {

const glm::vec4 COLOR_RED(1, 0, 0, 1);
const glm::vec4 COLOR_GREEN(0, 1, 0, 1);

const int GRID_TEXTURE_SIZE = 100;

const int LINE_WIDTH = 10;

TileMapSystem::TileMapSystem()
    : System("TileMapSystem"), m_operation(Operation::ADD_ENTITY) {
    m_outline_texture =
        Texture::Create(GRID_TEXTURE_SIZE, GRID_TEXTURE_SIZE,
                        TextureSpec(1, TextureType::TEX_2D, DataFormat::RGBA,
                                    DataFormatType::UBYTE));
    size_t pixel_size =
        4 * m_outline_texture->GetWidth() * m_outline_texture->GetHeight();
    void *data = malloc(pixel_size);
    memset(data, 0xff, pixel_size);
    m_outline_texture->SetPixels(0, 0, 0, m_outline_texture->GetWidth(),
                                 m_outline_texture->GetHeight(), 1, data);
    memset(data, 0x00, pixel_size);
    m_outline_texture->SetPixels(
        LINE_WIDTH, LINE_WIDTH, 0,
        m_outline_texture->GetWidth() - LINE_WIDTH * 2,
        m_outline_texture->GetHeight() - LINE_WIDTH * 2, 1, data);

    free(data);
}

void TileMapSystem::OnTick(float) {}

void TileMapSystem::OnPush() {}

void TileMapSystem::OnPop() {}

void TileMapSystem::OnImGui() {
    ImGui::Begin("TileMap System");
    {
        ImGui::Checkbox("Outline", &m_draw_outline);
        if (ImGui::RadioButton("Add Sprite",
                               reinterpret_cast<int *>(&m_operation),
                               Operation::ADD_ENTITY)) {
            m_brush = m_map->GetTexture();
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Clear Sprite",
                               reinterpret_cast<int *>(&m_operation),
                               Operation::REMOVE_ENTITY)) {
            m_brush = nullptr;
        }
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
            m_map_id = asset->LoadAsset<TileMap>(m_fileDialogInfo.result_path);
            m_map = asset->Get<TileMap>(m_map_id);
            m_brush_uvs = m_map->GetTileUVs(glm::vec2(0));
            m_brush = m_map->GetTexture();
        }
        if (m_map) {
            glm::ivec2 size = m_map->GetTileSize();
            ImGui::TextUnformatted("Tile Size:");
            if (ImGui::InputInt2("##Size", &size.x)) {
                m_map->SetTileSize(size);
            }

            ImGui::DrawTileMap(*m_map, m_brush_uvs);
        }
    }
    ImGui::End();
}

void TileMapSystem::OnRender() {
    if (m_map == nullptr) return;

    const glm::ivec2 TILE_SIZE = m_map->GetLayout().GetTileSize();
    device->SetTarget(renderer->GetDefaultTarget());
    device->Disable(SD::Operation::DEPTH_TEST);
    renderer->Begin(*scene->GetCamera());

    if (m_brush) {
        renderer->DrawTexture(
            m_brush, m_brush_uvs,
            glm::vec3(m_map->GetLayout().MapTileToWorld(m_select_tile_pos),
                      0.f),
            glm::quat(1.0f, 0.f, 0.f, 0.f), TILE_SIZE);
    }
    for (const auto &[pos, uv] : m_map->GetLayout().GetTiles()) {
        renderer->DrawTexture(
            m_map->GetTexture(), uv,
            glm::vec3(m_map->GetLayout().MapTileToWorld(pos), 0),
            glm::quat(1, 0, 0, 0), TILE_SIZE);
    }
    if (m_draw_outline) {
        RenderOutline();
    }
    renderer->End();
    device->Enable(SD::Operation::DEPTH_TEST);
}

void TileMapSystem::RenderOutline() {
    int render_width = renderer->GetDefaultTarget().GetWidth();
    int render_height = renderer->GetDefaultTarget().GetHeight();
    const glm::ivec2 TILE_SIZE = m_map->GetLayout().GetTileSize();
    const glm::ivec2 TILE_CNT(std::ceil(render_width / TILE_SIZE.x),
                              std::ceil(render_height / TILE_SIZE.y));
    const glm::vec2 TEX_SIZE = TILE_CNT * TILE_SIZE;

    const glm::vec3 pos = scene->GetCamera()->GetWorldPosition();
    glm::vec2 uv_origin(pos.x / TILE_SIZE.x, -pos.y / TILE_SIZE.y);
    renderer->DrawTexture(
        m_outline_texture, {uv_origin, glm::vec2(TILE_CNT) + uv_origin},
        glm::vec3(TEX_SIZE.x / 2.f + pos.x - TILE_SIZE.x / 2.f -
                      TILE_SIZE.x * std::ceil(TILE_CNT.x / 2.f),
                  -TEX_SIZE.y / 2.f + pos.y + TILE_SIZE.y / 2.f +
                      TILE_SIZE.y * std::ceil(TILE_CNT.y / 2.f),
                  0),
        glm::quat(1.0f, 0.f, 0.f, 0.f), TEX_SIZE, glm::vec4(1, 1, 1, 0.7));
    // Draw selection
    glm::vec4 select_color(1.0);
    switch (m_operation) {
        case Operation::ADD_ENTITY: {
            select_color = COLOR_GREEN;
        } break;
        case Operation::REMOVE_ENTITY: {
            select_color = COLOR_RED;
        } break;
    }
    renderer->DrawTexture(
        m_outline_texture, {glm::vec2(0), glm::vec2(1)},
        glm::vec3(m_map->GetLayout().MapTileToWorld(m_select_tile_pos), 0),
        glm::quat(1.0f, 0.f, 0.f, 0.f), TILE_SIZE, select_color);
}

void TileMapSystem::SelectWorldPos(const glm::vec2 &world) {
    if (m_map) {
        m_select_tile_pos = m_map->GetLayout().MapWorldToTile(world);
    }
}

void TileMapSystem::ApplyActionAtPos() {
    switch (m_operation) {
        case Operation::ADD_ENTITY: {
            if (m_map) {
                SD_TRACE("add tile at: {}", m_select_tile_pos);
                if (m_map->GetLayout().Has(m_select_tile_pos)) {
                    m_map->GetLayout().Clear(m_select_tile_pos);
                }
                m_map->GetLayout().Add(m_select_tile_pos, m_brush_uvs);
            }
        } break;
        case Operation::REMOVE_ENTITY: {
            if (m_map) {
                SD_TRACE("clear tile at: {}", m_select_tile_pos);
                if (m_map->GetLayout().Has(m_select_tile_pos)) {
                    m_map->GetLayout().Clear(m_select_tile_pos);
                }
            }
        } break;
    }
}

}  // namespace SD
