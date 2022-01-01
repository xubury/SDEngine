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

void TileMapSystem::OnTick(float) {}

void TileMapSystem::OnInit() { m_brush.scene = scene.get(); }

void TileMapSystem::OnPush() {}

void TileMapSystem::OnPop() {}

void TileMapSystem::OnImGui() {
    if (m_viewport && m_viewport->IsHover()) {
        glm::vec2 clip = m_viewport->MapScreenToClip(
            glm::ivec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y));
        if (std::abs(clip.x) > 1 || std::abs(clip.y) > 1) {
            return;
        }
        m_brush.SetRay(scene->GetCamera()->ComputeCameraRay(clip));
        if (ImGui::IsMouseClicked(0)) {
            switch (m_operation) {
                default: {
                } break;
                case Operation::ADD_ENTITY: {
                    m_brush.Output();
                } break;
                case Operation::REMOVE_ENTITY: {
                    m_brush.Clear();
                } break;
            }
        } else if (ImGui::IsMouseClicked(1)) {
            m_operation = Operation::NONE;
        }
    }
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

        int priority = m_brush.GetPriority();
        ImGui::TextUnformatted("Brush Prioirty");
        ImGui::SameLine();
        if (ImGui::InputInt("##Priority", &priority)) {
            m_brush.SetPriority(priority);
        }
        glm::ivec2 size = m_brush.canvas.GetTileSize();
        ImGui::TextUnformatted("Cavnas Tile Size:");
        ImGui::SameLine();
        if (ImGui::InputInt2("##Canvas Tile Size", &size.x)) {
            m_brush.canvas.SetTileSize(size);
        }

        auto sprite = asset->Get<Sprite>(m_brush.sprite_id);
        if (sprite) {
            ImGui::DrawTileTexture(*sprite->GetTexture(), m_brush.uvs,
                                   m_brush.count, m_brush.pivot);
        }
    }
    ImGui::End();
}

void TileMapSystem::OnRender() {
    device->SetTarget(renderer->GetDefaultTarget());
    device->Disable(SD::Operation::DEPTH_TEST);
    renderer->Begin(*scene->GetCamera());

    // draw brush & outline
    const glm::ivec2 TILE_SIZE = m_brush.canvas.GetTileSize();

    const glm::vec2 BRUSH_SIZE = TILE_SIZE * m_brush.count;
    glm::vec3 world;
    if (m_brush.GetSelectPos(world)) {
        if (m_operation == Operation::ADD_ENTITY) {
            auto sprite = asset->Get<Sprite>(m_brush.sprite_id);
            if (sprite) {
                renderer->DrawTexture(sprite->GetTexture(), m_brush.uvs, world,
                                      glm::quat(), BRUSH_SIZE);
            }
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
            renderer->DrawQuad(world, glm::quat(), BRUSH_SIZE, select_color);
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
        glm::vec2 uv_origin(cam_pos.x / TILE_SIZE.x, -cam_pos.y / TILE_SIZE.y);
        const glm::vec3 outline_pos(
            TEX_SIZE.x / 2.f + cam_pos.x - TILE_SIZE.x / 2.f -
                TILE_SIZE.x * std::floor(TILE_CNT.x / 2.f),
            -TEX_SIZE.y / 2.f + cam_pos.y + TILE_SIZE.y / 2.f +
                TILE_SIZE.y * std::floor(TILE_CNT.y / 2.f),
            0);
        renderer->DrawTexture(
            m_outline_texture, {uv_origin, glm::vec2(TILE_CNT) + uv_origin},
            outline_pos, glm::quat(), TEX_SIZE, glm::vec4(1, 1, 1, 0.7));
    }
    renderer->End();
    device->Enable(SD::Operation::DEPTH_TEST);
}

}  // namespace SD
