#include "TileMapEditor.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Resource/ResourceManager.hpp"
#include "ECS/Component.hpp"

namespace SD {

const Vector4f color_red(1, 0, 0, 0.5);
const Vector4f color_green(0, 1, 0, 0.5);

TileMapEditor::TileMapEditor() : m_operation(Operation::None) {}

bool TileMapEditor::ManipulateScene(const Camera &camera)
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
    m_frame.size = m_brush.GetTileSize();
    if (m_brush.CastRay()) {
        if (ImGui::IsMouseClicked(0) &&
            ResourceManager::Get().Exist<Texture>(m_frame.texture_id)) {
            return m_operation == Operation::AddEntity;
        }
        else if (ImGui::IsMouseClicked(1)) {
            m_operation = Operation::None;
        }
    }
    return false;
}

void TileMapEditor::ImGui()
{
    auto &resource = ResourceManager::Get();
    ImGui::Begin("TileMap Editor");
    {
        using underlying = std::underlying_type<Operation>::type;
        ImGui::RadioButton("None", reinterpret_cast<underlying *>(&m_operation),
                           static_cast<underlying>(Operation::None));
        ImGui::SameLine();
        if (ImGui::RadioButton("Add Sprite",
                               reinterpret_cast<underlying *>(&m_operation),
                               static_cast<underlying>(Operation::AddEntity))) {
            m_brush.color = color_green;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton(
                "Clear Sprite", reinterpret_cast<underlying *>(&m_operation),
                static_cast<underlying>(Operation::RemoveEntity))) {
            m_brush.color = color_red;
        }
        ImGui::DrawTextureAssetSelection(&m_frame.texture_id);
        ImGui::InputInt("Priority", &m_frame.priority);

        if (resource.Exist<Texture>(m_frame.texture_id)) {
            ImGui::DrawTileTexture(*resource.GetResource<Texture>(m_frame.texture_id),
                                   m_brush.tile_size, m_frame.uvs,
                                   &m_brush.count, &m_brush.pivot);
        }
    }
    ImGui::End();
    m_brush.is_painting = m_operation != Operation::None;
}

}  // namespace SD
