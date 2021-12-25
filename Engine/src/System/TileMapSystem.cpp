#include "System/TileMapSystem.hpp"
#include "Core/Input.hpp"

namespace SD {

TileMapSystem::TileMapSystem()
    : System("TileMapSystem"), m_operation(Operation::ADD_SPRITE) {}

void TileMapSystem::OnTick(float) {}

void TileMapSystem::OnPush() {}

void TileMapSystem::OnPop() {}

void TileMapSystem::OnImGui() {
    ImGui::Begin("TileMap System");
    {
        ImGui::Checkbox("Outline", &m_draw_outline);
        if (ImGui::RadioButton("Add Sprite",
                               reinterpret_cast<int *>(&m_operation),
                               Operation::ADD_SPRITE)) {
            m_selected_sprite.SetTexture(m_map.GetTexture());
        }

        ImGui::SameLine();
        if (ImGui::RadioButton("Clear Sprite",
                               reinterpret_cast<int *>(&m_operation),
                               Operation::CLEAR_SPRITE)) {
            m_selected_sprite.SetTexture(nullptr);
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
            m_map_id = asset->LoadAsset<Bitmap>(m_fileDialogInfo.result_path);
            auto bitmap = asset->Get<Bitmap>(m_map_id);
            auto texture = Texture::Create(
                bitmap->Width(), bitmap->Height(),
                TextureSpec(
                    1, TextureType::TEX_2D,
                    bitmap->HasAlpha() ? DataFormat::RGBA : DataFormat::RGB,
                    DataFormatType::UBYTE, TextureWrap::EDGE));
            texture->SetPixels(0, 0, 0, bitmap->Width(), bitmap->Height(), 1,
                               bitmap->Data());
            m_map.SetTexture(texture);
        }
        glm::ivec2 size = m_map.GetTileSize();
        ImGui::TextUnformatted("Tile Size:");
        if (ImGui::InputInt2("##Size", &size.x)) {
            m_map.SetTileSize(size);
        }

        std::array<glm::vec2, 2> uvs = m_selected_sprite.GetUVs();
        if (ImGui::DrawTileMap(m_map, uvs)) {
            m_selected_sprite.Set(m_map.GetTexture(), uvs);
        }
    }
    ImGui::End();
}

void TileMapSystem::OnRender() {
    const glm::ivec2 TILE_SIZE = m_layout.GetTileSize();
    device->SetTarget(renderer->GetDefaultTarget());
    device->Disable(SD::Operation::DEPTH_TEST);
    renderer->Begin(*scene->GetCamera());

    if (m_selected_sprite.Valid()) {
        renderer->DrawTexture(
            m_selected_sprite.GetTexture(), m_selected_sprite.GetUVs(),
            glm::vec3(m_layout.MapTileToWorld(m_select_tile_pos), 0.f),
            glm::quat(1.0f, 0.f, 0.f, 0.f), TILE_SIZE);
    }
    for (auto &[pos, sprite] : m_layout.GetSprites()) {
        renderer->DrawTexture(sprite.GetTexture(), sprite.GetUVs(),
                              glm::vec3(m_layout.MapTileToWorld(pos), 0.f),
                              glm::quat(1.0f, 0.f, 0.f, 0.f), TILE_SIZE);
    }
    if (m_draw_outline) {
        RenderOutline();
    }
    renderer->End();
    device->Enable(SD::Operation::DEPTH_TEST);
}

void TileMapSystem::RenderOutline() {
    const glm::ivec2 TILE_SIZE = m_layout.GetTileSize();

    int render_width = renderer->GetDefaultTarget().GetWidth();
    int render_height = renderer->GetDefaultTarget().GetHeight();
    const glm::ivec2 TILE_CNT(render_width * TILE_SIZE.x,
                              render_height * TILE_SIZE.y);
    const glm::vec3 pos = scene->GetCamera()->GetWorldPosition();
    glm::vec2 uv_origin(pos.x / TILE_SIZE.x, -pos.y / TILE_SIZE.y);
    renderer->DrawTexture(
        m_layout.GetOutlineTexture(),
        {uv_origin, glm::vec2(TILE_CNT) + uv_origin},
        glm::vec3(-TILE_SIZE.x / 2.f + pos.x, -TILE_SIZE.y / 2.f + pos.y, 0),
        glm::quat(1.0f, 0.f, 0.f, 0.f), TILE_CNT * TILE_SIZE,
        glm::vec4(1, 1, 1, 0.7));
    renderer->DrawTexture(
        m_layout.GetOutlineTexture(), {glm::vec2(0), glm::vec2(1)},
        glm::vec3(m_layout.MapTileToWorld(m_select_tile_pos), 0),
        glm::quat(1.0f, 0.f, 0.f, 0.f), TILE_SIZE, glm::vec4(0, 1, 0, 1));
}

void TileMapSystem::SelectWorldPos(const glm::vec2 &world) {
    m_select_tile_pos = m_layout.MapWorldToTile(world);
}

void TileMapSystem::ApplyActionAtPos() {
    switch (m_operation) {
        case Operation::ADD_SPRITE: {
            if (m_selected_sprite.Valid()) {
                SD_TRACE("add tile at: {}", m_select_tile_pos);
                m_layout.SetSprite(m_select_tile_pos, m_selected_sprite);
            }
        } break;
        case Operation::CLEAR_SPRITE: {
            SD_TRACE("clear tile at: {}", m_select_tile_pos);
            m_layout.ClearSprite(m_select_tile_pos);
        } break;
    }
}

}  // namespace SD
