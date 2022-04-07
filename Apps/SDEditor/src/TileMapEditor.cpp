#include "TileMapEditor.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Asset/AssetStorage.hpp"
#include "ECS/Component.hpp"

namespace SD {

const Vector4f color_red(1, 0, 0, 0.5);
const Vector4f color_green(0, 1, 0, 0.5);

const int grid_texture_size = 100;
const int line_width = 5;

TileMapEditor::TileMapEditor()
    : m_draw_outline(true), m_operation(Operation::None)
{
    m_outline_texture = Texture::Create(
        grid_texture_size, grid_texture_size, 0, MultiSampleLevel::None,
        TextureType::Normal, DataFormat::RGBA8, TextureWrap::Repeat);
    size_t pixel_size =
        4 * m_outline_texture->GetWidth() * m_outline_texture->GetHeight();
    void *data = malloc(pixel_size);
    memset(data, 0xff, pixel_size);
    m_outline_texture->SetPixels(0, 0, 0, m_outline_texture->GetWidth(),
                                 m_outline_texture->GetHeight(), 1, data);
    memset(data, 0x00, pixel_size);
    m_outline_texture->SetPixels(
        line_width, line_width, 0,
        m_outline_texture->GetWidth() - line_width * 2,
        m_outline_texture->GetHeight() - line_width * 2, 1, data);

    free(data);
}

bool TileMapEditor::ManipulateScene(const Camera &camera, Vector3f &world)
{
    if (!ImGui::IsWindowHovered()) {
        return false;
    }
    const auto &wsize = ImGui::GetContentRegionAvail();
    const auto &min_region = ImGui::GetWindowContentRegionMin();
    const auto &wpos = ImGui::GetWindowPos();
    const Vector2i viewport_pos = {min_region.x + wpos.x,
                                   min_region.y + wpos.y};
    m_viewport.SetPos(viewport_pos.x, viewport_pos.y);
    m_viewport.SetSize(wsize.x, wsize.y);
    auto [mouse_x, mouse_y] = ImGui::GetMousePos();
    Vector2f clip = m_viewport.MapScreenToClip(Vector2i(mouse_x, mouse_y));
    if (std::abs(clip.x) > 1 || std::abs(clip.y) > 1) {
        return false;
    }
    m_brush.SetRay(camera.ComputeCameraRay(clip));
    if (ImGui::IsMouseClicked(0) &&
        AssetStorage::Get().Exists<TextureAsset>(m_frame.texture_id)) {
        switch (m_operation) {
            default: {
            } break;
            case Operation::AddEntity: {
                return m_brush.GetSelectTile(world, m_frame.size);
            } break;
            case Operation::RemoveEntity: {
                // m_brush.Clear();
            } break;
        }
    }
    else if (ImGui::IsMouseClicked(1)) {
        m_operation = Operation::None;
    }
    return false;
}

void TileMapEditor::ImGui()
{
    ImGui::Begin("TileMap Editor");
    {
        using underlying = std::underlying_type<Operation>::type;
        ImGui::Checkbox("Outline", &m_draw_outline);
        ImGui::RadioButton("None", reinterpret_cast<underlying *>(&m_operation),
                           static_cast<underlying>(Operation::None));
        ImGui::SameLine();
        ImGui::RadioButton("Add Sprite",
                           reinterpret_cast<underlying *>(&m_operation),
                           static_cast<underlying>(Operation::AddEntity));
        ImGui::SameLine();
        ImGui::RadioButton("Clear Sprite",
                           reinterpret_cast<underlying *>(&m_operation),
                           static_cast<underlying>(Operation::RemoveEntity));
        ImGui::DrawTextureAssetSelection(AssetStorage::Get(),
                                         &m_frame.texture_id);
        ImGui::InputInt("Priority", &m_frame.priority);

        if (AssetStorage::Get().Exists<TextureAsset>(m_frame.texture_id)) {
            ImGui::DrawTileTexture(
                *AssetStorage::Get()
                     .GetAsset<TextureAsset>(m_frame.texture_id)
                     ->GetTexture(),
                m_brush.tile_size, m_frame.uvs, &m_brush.count, &m_brush.pivot);
        }
    }
    ImGui::End();
}

void TileMapEditor::Render(const Camera &camera)
{
    const int index = 0;
    RenderOperation op;
    op.depth_test = false;
    Renderer::BeginRenderSubpass(RenderSubpassInfo{&index, 1, op});
    Renderer2D::Begin();

    Vector3f world;
    if (m_brush.GetSelectTile(world, m_frame.size)) {
        if (m_operation == Operation::AddEntity) {
            if (AssetStorage::Get().Exists<TextureAsset>(m_frame.texture_id)) {
                Renderer2D::DrawTexture(
                    *AssetStorage::Get()
                         .GetAsset<TextureAsset>(m_frame.texture_id)
                         ->GetTexture(),
                    m_frame.uvs, world, Quaternion(), m_frame.size);
            }
        }
        // Draw selection
        Vector4f select_color(1.0);
        switch (m_operation) {
            case Operation::None:
            case Operation::AddEntity: {
                select_color = color_green;
            } break;
            case Operation::RemoveEntity: {
                select_color = color_red;
            } break;
        }
        if (m_operation != Operation::None) {
            Renderer2D::DrawQuad(world, Quaternion(), m_frame.size,
                                 select_color);
        }
    }

    // draw brush & outline
    const Vector2i &tile_size = m_brush.tile_size;
    if (m_draw_outline) {
        auto &render_pass = Renderer::GetCurrentRenderPass();
        int render_width = render_pass.viewport_width;
        int render_height = render_pass.viewport_height;
        const Vector2i tile_cnt(
            std::ceil(static_cast<float>(render_width) / tile_size.x) + 1,
            std::ceil(static_cast<float>(render_height) / tile_size.y) + 1);
        const Vector2f tex_size = tile_cnt * tile_size;

        const Vector3f cam_pos = camera.GetWorldPosition();
        Vector2f uv_origin(cam_pos.x / tile_size.x, -cam_pos.y / tile_size.y);
        const Vector3f outline_pos(
            tex_size.x / 2.f + cam_pos.x - tile_size.x / 2.f -
                tile_size.x * std::floor(tile_cnt.x / 2.f),
            -tex_size.y / 2.f + cam_pos.y + tile_size.y / 2.f +
                tile_size.y * std::floor(tile_cnt.y / 2.f),
            0);
        Renderer2D::DrawTexture(
            *m_outline_texture, {uv_origin, Vector2f(tile_cnt) + uv_origin},
            outline_pos, Quaternion(), tex_size, Vector4f(1, 1, 1, 0.7));
    }
    Renderer2D::End();
    Renderer::EndRenderSubpass();
}

}  // namespace SD
