#include "System/TileMapSystem.hpp"
#include "Core/Input.hpp"

namespace SD {

const glm::vec4 COLOR_RED(1, 0, 0, 0.5);
const glm::vec4 COLOR_GREEN(0, 1, 0, 0.5);

const int GRID_TEXTURE_SIZE = 100;
const int LINE_WIDTH = 5;

TileMapSystem::TileMapSystem()
    : System("TileMapSystem"),
      m_viewport(nullptr),
      m_file_dialog_open(false),
      m_draw_outline(true),
      m_operation(Operation::NONE) {
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
        auto &layout =
            scene->GetSelectedEntity().GetComponent<TileMapComponent>().tiles;
        auto &transform = scene->GetSelectedEntity()
                              .GetComponent<TransformComponent>()
                              .transform;
        if (layout.GetVisible()) {
            Math::Ray ray = scene->GetCamera()->ComputeCameraRay(clip);
            Math::Plane plane(transform.GetWorldFront(),
                              transform.GetWorldPosition());
            glm::vec3 world;
            if (Math::IntersectRayPlane(ray, plane, world)) {
                m_select_tile_pos = layout.MapWorldToTile(world, &transform);
            }
        }
        if (Input::IsMousePressed(MouseButton::LEFT)) {
            ApplyAction(layout);
        } else if (Input::IsMousePressed(MouseButton::RIGHT)) {
            m_operation = Operation::NONE;
        }
    }
}

void TileMapSystem::OnPush() {}

void TileMapSystem::OnPop() {}

void TileMapSystem::OnImGui() {
    ImGui::Begin("TileMap System");
    {
        ImGui::Checkbox("Outline", &m_draw_outline);
        ImGui::RadioButton("None", reinterpret_cast<int *>(&m_operation),
                           Operation::NONE);
        ImGui::SameLine();
        ImGui::RadioButton("Add Sprite", reinterpret_cast<int *>(&m_operation),
                           Operation::ADD_ENTITY);
        ImGui::SameLine();
        ImGui::RadioButton("Clear Sprite",
                           reinterpret_cast<int *>(&m_operation),
                           Operation::REMOVE_ENTITY);
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
            m_fileDialogInfo.regex_match = IMG_FILTER;
        }
        if (ImGui::FileDialog(&m_file_dialog_open, &m_fileDialogInfo)) {
            m_brush.sprite_id =
                asset->LoadAsset<Sprite>(m_fileDialogInfo.result_path);
        }

        auto sprite = asset->Get<Sprite>(m_brush.sprite_id);
        if (sprite) {
            ImGui::DrawTileTexture(*sprite->GetTexture(), m_brush.uvs,
                                   m_brush.count);
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
        auto &transform = entity.GetComponent<TransformComponent>().transform;
        const glm::ivec2 TILE_SIZE = layout.GetTileSize();

        const glm::vec2 BRUSH_SIZE = TILE_SIZE * m_brush.count;
        glm::vec3 select_pos(m_select_tile_pos, 0);
        glm::vec3 offset(BRUSH_SIZE.x / 2 - TILE_SIZE.x / 2.f,
                         -BRUSH_SIZE.y / 2 + TILE_SIZE.y / 2.f, 0);
        select_pos = layout.MapTileToWorld(select_pos, &transform) +
                     transform.ToWorldVector(offset);
        if (m_operation == Operation::ADD_ENTITY) {
            auto sprite = asset->Get<Sprite>(m_brush.sprite_id);
            if (sprite) {
                renderer->DrawTexture(sprite->GetTexture(), m_brush.uvs,
                                      select_pos, transform.GetWorldRotation(),
                                      BRUSH_SIZE);
            }
        }

        if (m_draw_outline) {
            int render_width = renderer->GetDefaultTarget().GetWidth();
            int render_height = renderer->GetDefaultTarget().GetHeight();
            const glm::ivec2 TILE_CNT(
                std::ceil(static_cast<float>(render_width) / TILE_SIZE.x) + 1,
                std::ceil(static_cast<float>(render_height) / TILE_SIZE.y) + 1);
            const glm::vec2 TEX_SIZE = TILE_CNT * TILE_SIZE;

            const glm::vec3 cam_pos = scene->GetCamera()->GetWorldPosition();
            glm::vec2 uv_origin(cam_pos.x / TILE_SIZE.x,
                                -cam_pos.y / TILE_SIZE.y);
            const glm::vec3 outline_pos(
                TEX_SIZE.x / 2.f + cam_pos.x - TILE_SIZE.x / 2.f -
                    TILE_SIZE.x * std::floor(TILE_CNT.x / 2.f),
                -TEX_SIZE.y / 2.f + cam_pos.y + TILE_SIZE.y / 2.f +
                    TILE_SIZE.y * std::floor(TILE_CNT.y / 2.f),
                0);
            renderer->DrawTexture(m_outline_texture,
                                  {uv_origin, glm::vec2(TILE_CNT) + uv_origin},
                                  transform.ToWorldSpace(outline_pos),
                                  transform.GetWorldRotation(), TEX_SIZE,
                                  glm::vec4(1, 1, 1, 0.7));
        }
        // Draw selection
        glm::vec4 select_color(1.0);
        switch (m_operation) {
            case Operation::NONE:
            case Operation::ADD_ENTITY: {
                select_color = COLOR_GREEN;
            } break;
            case Operation::REMOVE_ENTITY: {
                select_color = COLOR_RED;
            } break;
        }
        if (m_operation != Operation::NONE) {
            renderer->DrawQuad(select_pos, transform.GetWorldRotation(),
                               BRUSH_SIZE, select_color);
        }
    }
    renderer->End();
    device->Enable(SD::Operation::DEPTH_TEST);
}

void TileMapSystem::ApplyAction(TileLayout<Tile> &layout) {
    switch (m_operation) {
        default: {
        } break;
        case Operation::ADD_ENTITY: {
            auto tile_uvs = m_brush.ComputeTileUV();
            for (int y = 0; y < m_brush.count.y; ++y) {
                for (int x = 0; x < m_brush.count.x; ++x) {
                    glm::ivec2 pos(m_select_tile_pos.x + x,
                                   m_select_tile_pos.y - y);
                    SD_TRACE("add tile at: {}", pos);
                    if (layout.Has(pos)) {
                        layout.Clear(pos);
                    }
                    layout.Add(pos, {m_brush.sprite_id, tile_uvs[y][x]});
                }
            }
        } break;
        case Operation::REMOVE_ENTITY: {
            for (int y = 0; y < m_brush.count.y; ++y) {
                for (int x = 0; x < m_brush.count.x; ++x) {
                    glm::ivec2 pos(m_select_tile_pos.x + x,
                                   m_select_tile_pos.y + y);
                    if (layout.Has(pos)) {
                        SD_TRACE("clear tile at: {}", pos);
                        layout.Clear(pos);
                    }
                }
            }
        } break;
    }
}

}  // namespace SD
