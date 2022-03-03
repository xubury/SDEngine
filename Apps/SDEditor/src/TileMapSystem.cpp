#include "TileMapSystem.hpp"
#include "Core/Input.hpp"
#include "Core/Window.hpp"
#include "Renderer/Renderer.hpp"
#include "Asset/AssetStorage.hpp"

namespace SD {

const glm::vec4 COLOR_RED(1, 0, 0, 0.5);
const glm::vec4 COLOR_GREEN(0, 1, 0, 0.5);

const int GRID_TEXTURE_SIZE = 100;
const int LINE_WIDTH = 5;

TileMapSystem::TileMapSystem()
    : System("TileMapSystem"),
      m_file_dialog_open(false),
      m_texture_asset(nullptr),
      m_priority(0),
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

void TileMapSystem::OnPush() {
    m_viewport_size_handler = EventSystem::Get().Register<ViewportSizeEvent>(
        [this](const ViewportSizeEvent &e) {
            m_viewport.SetSize(e.width, e.height);
        });
    m_viewport_pos_handler = EventSystem::Get().Register<ViewportPosEvent>(
        [this](const ViewportPosEvent &e) { m_viewport.SetSize(e.x, e.y); });
    m_viewport_state_handler = EventSystem::Get().Register<ViewportStateEvent>(
        [this](const ViewportStateEvent &e) {
            m_viewport.SetFocus(e.is_focus);
            m_viewport.SetHover(e.is_hover);
        });
}

void TileMapSystem::OnPop() {
    EventSystem::Get().RemoveHandler(m_viewport_size_handler);
    EventSystem::Get().RemoveHandler(m_viewport_pos_handler);
    EventSystem::Get().RemoveHandler(m_viewport_state_handler);
}

void TileMapSystem::OnImGui() {
    if (m_viewport.IsHover()) {
        glm::vec2 clip = m_viewport.MapScreenToClip(
            glm::ivec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y));
        if (std::abs(clip.x) > 1 || std::abs(clip.y) > 1) {
            return;
        }
        m_brush.SetRay(scene->GetCamera()->ComputeCameraRay(clip));
        if (ImGui::IsMouseClicked(0) && m_texture_asset) {
            switch (m_operation) {
                default: {
                } break;
                case Operation::ADD_ENTITY: {
                    glm::vec3 world;
                    if (m_brush.GetSelectPos(world)) {
                        Entity child = scene->CreateEntity("Tile");
                        auto &comp = child.AddComponent<SpriteComponent>();
                        auto &frame = comp.frame;
                        child.GetComponent<TransformComponent>()
                            .SetWorldPosition(world);
                        frame.texture_id = m_texture_asset->GetId();
                        frame.texture_path = m_texture_asset->GetPath();
                        frame.uvs = m_uvs;
                        frame.size = m_brush.tile_size * m_brush.count;
                        child.GetComponent<PriorityComponent>().priority =
                            m_priority;
                    }
                } break;
                case Operation::REMOVE_ENTITY: {
                    // m_brush.Clear();
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
        // ImGui::InputText("##Path", m_texture_path.data(),
        // m_texture_path.size(),
        //                  ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        if (ImGui::Button("Open")) {
            m_file_dialog_open = true;
            m_fileDialogInfo.type = ImGuiFileDialogType::OPEN_FILE;
            m_fileDialogInfo.title = "Open File";
            m_fileDialogInfo.file_name = "";
            m_fileDialogInfo.directory_path =
                AssetStorage::Get().GetDirectory();
            m_fileDialogInfo.regex_match = IMG_FILTER;
        }
        if (ImGui::FileDialog(&m_file_dialog_open, &m_fileDialogInfo)) {
            try {
                m_texture_asset = AssetStorage::Get().LoadAsset<TextureAsset>(
                    m_fileDialogInfo.result_path.string());
            } catch (const Exception &e) {
                SD_CORE_ERROR("Error loading sprite: {}", e.what());
            }
        }

        ImGui::TextUnformatted("Brush Prioirty");
        ImGui::SameLine();
        ImGui::InputInt("##Priority", &m_priority);

        if (m_texture_asset) {
            ImGui::DrawTileTexture(*m_texture_asset->GetTexture(),
                                   m_brush.tile_size, m_uvs, &m_brush.count,
                                   &m_brush.pivot);
        }
    }
    ImGui::End();
}

void TileMapSystem::OnRender() {
    Device::Get().SetFramebuffer(Renderer::Get().GetFramebuffer());
    Device::Get().DrawBuffer(Renderer::Get().GetFramebuffer(),
                             0);  // only draw colors
    Device::Get().Disable(SD::Operation::DEPTH_TEST);
    Renderer::Get().Begin(*scene->GetCamera());

    // draw brush & outline
    const glm::ivec2 &tile_size = m_brush.tile_size;
    const glm::vec2 &brush_size = tile_size * m_brush.count;
    glm::vec3 world;
    if (m_brush.GetSelectPos(world)) {
        if (m_operation == Operation::ADD_ENTITY) {
            if (m_texture_asset) {
                Renderer::Get().DrawTexture(*m_texture_asset->GetTexture(),
                                            m_uvs, world, glm::quat(),
                                            brush_size);
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
            Renderer::Get().DrawQuad(world, glm::quat(), brush_size,
                                     select_color);
        }
    }

    if (m_draw_outline) {
        int render_width = Renderer::Get().GetFramebuffer()->GetWidth();
        int render_height = Renderer::Get().GetFramebuffer()->GetHeight();
        const glm::ivec2 tile_cnt(
            std::ceil(static_cast<float>(render_width) / tile_size.x) + 1,
            std::ceil(static_cast<float>(render_height) / tile_size.y) + 1);
        const glm::vec2 tex_size = tile_cnt * tile_size;

        const glm::vec3 cam_pos = scene->GetCamera()->GetWorldPosition();
        glm::vec2 uv_origin(cam_pos.x / tile_size.x, -cam_pos.y / tile_size.y);
        const glm::vec3 outline_pos(
            tex_size.x / 2.f + cam_pos.x - tile_size.x / 2.f -
                tile_size.x * std::floor(tile_cnt.x / 2.f),
            -tex_size.y / 2.f + cam_pos.y + tile_size.y / 2.f +
                tile_size.y * std::floor(tile_cnt.y / 2.f),
            0);
        Renderer::Get().DrawTexture(
            *m_outline_texture, {uv_origin, glm::vec2(tile_cnt) + uv_origin},
            outline_pos, glm::quat(), tex_size, glm::vec4(1, 1, 1, 0.7));
    }
    Renderer::Get().End();
    Device::Get().Enable(SD::Operation::DEPTH_TEST);
}

}  // namespace SD
