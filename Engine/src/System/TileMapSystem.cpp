#include "System/TileMapSystem.hpp"
#include "Core/Input.hpp"

namespace SD {

const glm::vec4 COLOR_RED(1, 0, 0, 1);
const glm::vec4 COLOR_GREEN(0, 1, 0, 1);

const int GRID_TEXTURE_SIZE = 100;

const int LINE_WIDTH = 10;

TileMapSystem::TileMapSystem()
    : System("TileMapSystem"),
      m_viewport(nullptr),
      m_sprite_id(0),
      m_file_dialog_open(false),
      m_draw_outline(true),
      m_operation(Operation::ADD_ENTITY) {
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

void TileMapSystem::OnTick(float) {
    Entity entity = scene->GetSelectedEntity();
    if (m_viewport && m_viewport->IsHover() && m_viewport->IsFocus() &&
        entity && entity.HasComponent<TileMapComponent>()) {
        glm::vec2 clip = m_viewport->MapScreenToClip(Input::GetMouseCoord());
        if (std::abs(clip.x) > 1 || std::abs(clip.y) > 1) {
            return;
        }
        Math::Ray ray = scene->GetCamera()->ComputeCameraRay(clip);
        Math::Plane plane(glm::vec3(0, 0, 1), glm::vec3(0));
        glm::vec3 world;
        if (Math::IntersectRayPlane(ray, plane, world)) {
            auto &layout = scene->GetSelectedEntity()
                               .GetComponent<TileMapComponent>()
                               .tiles;
            m_select_tile_pos = layout.MapWorldToTile(world);
            if (Input::IsMousePressed(MouseButton::LEFT)) {
                switch (m_operation) {
                    case Operation::ADD_ENTITY: {
                        float x_step = (m_brush.uvs[1].x - m_brush.uvs[0].x) /
                                       m_brush.cnt.x;
                        float y_step = (m_brush.uvs[1].y - m_brush.uvs[0].y) /
                                       m_brush.cnt.y;
                        for (int x = 0; x < m_brush.cnt.x; ++x) {
                            for (int y = 0; y < m_brush.cnt.y; ++y) {
                                std::array<glm::vec2, 2> uvs;
                                uvs[0] = m_brush.uvs[0] +
                                         glm::vec2(x * x_step, y * y_step);
                                uvs[1] = m_brush.uvs[0] +
                                         glm::vec2((x + 1) * x_step,
                                                   (y + 1) * y_step);
                                glm::ivec2 pos(m_select_tile_pos.x + x,
                                               m_select_tile_pos.y + y);
                                SD_TRACE("add tile at: {}", pos);
                                if (layout.Has(pos)) {
                                    layout.Clear(pos);
                                }
                                layout.Add(pos, {m_sprite_id, uvs});
                            }
                        }
                    } break;
                    case Operation::REMOVE_ENTITY: {
                        SD_TRACE("clear tile at: {}", m_select_tile_pos);
                        if (layout.Has(m_select_tile_pos)) {
                            layout.Clear(m_select_tile_pos);
                        }
                    } break;
                }
            }
        }
    }
}

void TileMapSystem::OnPush() {}

void TileMapSystem::OnPop() {}

void TileMapSystem::OnImGui() {
    ImGui::Begin("TileMap System");
    {
        ImGui::Checkbox("Outline", &m_draw_outline);
        if (ImGui::RadioButton("Add Sprite",
                               reinterpret_cast<int *>(&m_operation),
                               Operation::ADD_ENTITY)) {
            m_brush.sprite = asset->Get<Sprite>(m_sprite_id);
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Clear Sprite",
                               reinterpret_cast<int *>(&m_operation),
                               Operation::REMOVE_ENTITY)) {
            m_brush.sprite = nullptr;
        }
        std::string path = m_fileDialogInfo.result_path.string();
        ImGui::InputText("##Path", path.data(), path.size(),
                         ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        if (ImGui::Button("Open")) {
            m_file_dialog_open = true;
            m_fileDialogInfo.type = ImGuiFileDialogType::OPEN_FILE;
            m_fileDialogInfo.title = "Open File";
            m_fileDialogInfo.file_name = "";
            m_fileDialogInfo.directory_path = asset->GetRootPath();
        }
        if (ImGui::FileDialog(&m_file_dialog_open, &m_fileDialogInfo)) {
            m_sprite_id =
                asset->LoadAsset<Sprite>(m_fileDialogInfo.result_path);
            if (m_operation == Operation::ADD_ENTITY) {
                m_brush.sprite = asset->Get<Sprite>(m_sprite_id);
            }
        }

        if (m_brush.sprite) {
            ImGui::DrawTileTexture(*m_brush.sprite->GetTexture(), m_brush.uvs,
                                   m_brush.cnt);
        }
    }
    ImGui::End();
}

void TileMapSystem::OnRender() {
    device->SetTarget(renderer->GetDefaultTarget());
    device->Disable(SD::Operation::DEPTH_TEST);
    renderer->Begin(*scene->GetCamera());

    // draw brush & outline
    Entity entity = scene->GetSelectedEntity();
    if (entity && entity.HasComponent<TileMapComponent>()) {
        auto &layout = entity.GetComponent<TileMapComponent>().tiles;
        const glm::ivec2 TILE_SIZE = layout.GetTileSize();

        if (m_brush.sprite) {
            const glm::vec2 BRUSH_SIZE = TILE_SIZE * m_brush.cnt;
            glm::vec2 pos = layout.MapTileToWorld(m_select_tile_pos);
            pos.x += BRUSH_SIZE.x / 2 - TILE_SIZE.x / 2.f;
            pos.y -= BRUSH_SIZE.y / 2 - TILE_SIZE.y / 2.f;
            renderer->DrawTexture(m_brush.sprite->GetTexture(), m_brush.uvs,
                                  glm::vec3(pos, 0.f),
                                  glm::quat(1.0f, 0.f, 0.f, 0.f), BRUSH_SIZE);
        }

        if (m_draw_outline) {
            RenderOutline();
        }
    }
    renderer->End();
    device->Enable(SD::Operation::DEPTH_TEST);
}

void TileMapSystem::RenderOutline() {
    auto &layout =
        scene->GetSelectedEntity().GetComponent<TileMapComponent>().tiles;
    int render_width = renderer->GetDefaultTarget().GetWidth();
    int render_height = renderer->GetDefaultTarget().GetHeight();
    const glm::ivec2 TILE_SIZE = layout.GetTileSize();
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
        glm::vec3(layout.MapTileToWorld(m_select_tile_pos), 0),
        glm::quat(1.0f, 0.f, 0.f, 0.f), TILE_SIZE, select_color);
}

}  // namespace SD
