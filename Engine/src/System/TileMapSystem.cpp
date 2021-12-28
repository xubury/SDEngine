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

void TileMapSystem::OnTick(float) {
    Entity entity = scene->GetSelectedEntity();
    if (entity && entity.HasComponent<TileMapComponent>()) {
        auto &map =
            scene->GetSelectedEntity().GetComponent<TileMapComponent>().map;
        glm::vec2 clip = m_viewport.MapScreenToClip(Input::GetMouseCoord());
        if (std::abs(clip.x) > 1 || std::abs(clip.y) > 1) {
            return;
        }
        Math::Ray ray = scene->GetCamera()->ComputeCameraRay(clip);
        Math::Plane plane(glm::vec3(0, 0, 1), glm::vec3(0));
        glm::vec3 world;
        if (Math::IntersectRayPlane(ray, plane, world)) {
            auto &layout = map.GetLayout();
            m_select_tile_pos = layout.MapWorldToTile(world);
            if (Input::IsMousePressed(MouseButton::LEFT)) {
                switch (m_operation) {
                    case Operation::ADD_ENTITY: {
                        SD_TRACE("add tile at: {}", m_select_tile_pos);
                        if (layout.Has(m_select_tile_pos)) {
                            layout.Clear(m_select_tile_pos);
                        }
                        layout.Add(m_select_tile_pos, m_brush_uvs);
                        map.SetTexture(m_brush);
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
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Clear Sprite",
                               reinterpret_cast<int *>(&m_operation),
                               Operation::REMOVE_ENTITY)) {
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
            auto bitmap =
                asset->LoadAndGet<Bitmap>(m_fileDialogInfo.result_path);
            if (bitmap) {
                m_brush = Texture::Create(
                    bitmap->Width(), bitmap->Height(),
                    TextureSpec(
                        1, TextureType::TEX_2D,
                        bitmap->HasAlpha() ? DataFormat::RGBA : DataFormat::RGB,
                        DataFormatType::UBYTE, TextureWrap::EDGE));
                m_brush->SetPixels(0, 0, 0, bitmap->Width(), bitmap->Height(),
                                   1, bitmap->Data());
            }
        }

        if (m_brush) {
            static glm::ivec2 size(50);
            ImGui::TextUnformatted("Tile Size:");
            ImGui::InputInt2("##Size", &size.x);
            ImGui::DrawTileMap(*m_brush, size, m_brush_uvs);
        }
    }
    ImGui::End();
}

void TileMapSystem::OnRender() {
    if (!scene->GetSelectedEntity()) {
        return;
    }
    if (!scene->GetSelectedEntity().HasComponent<TileMapComponent>()) {
        return;
    }
    auto &map = scene->GetSelectedEntity().GetComponent<TileMapComponent>().map;
    auto &layout = map.GetLayout();
    const glm::ivec2 TILE_SIZE = layout.GetTileSize();
    device->SetTarget(renderer->GetDefaultTarget());
    device->Disable(SD::Operation::DEPTH_TEST);
    renderer->Begin(*scene->GetCamera());

    if (m_brush) {
        renderer->DrawTexture(
            m_brush, m_brush_uvs,
            glm::vec3(layout.MapTileToWorld(m_select_tile_pos), 0.f),
            glm::quat(1.0f, 0.f, 0.f, 0.f), TILE_SIZE);
    }

    auto tilemap_comp = scene->view<TileMapComponent>();
    tilemap_comp.each([this](const TileMapComponent &tilemap_comp) {
        auto &layout = tilemap_comp.map.GetLayout();
        for (const auto &[pos, uv] : layout.GetTiles()) {
            renderer->DrawTexture(tilemap_comp.map.GetTexture(), uv,
                                  glm::vec3(layout.MapTileToWorld(pos), 0),
                                  glm::quat(1, 0, 0, 0), layout.GetTileSize());
        }
    });
    if (m_draw_outline) {
        RenderOutline();
    }
    renderer->End();
    device->Enable(SD::Operation::DEPTH_TEST);
}

void TileMapSystem::RenderOutline() {
    auto &map = scene->GetSelectedEntity().GetComponent<TileMapComponent>().map;
    auto &layout = map.GetLayout();
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
