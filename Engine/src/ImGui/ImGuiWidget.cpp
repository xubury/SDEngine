#include "ImGui/ImGuiWidget.hpp"
#include "imgui.h"
#include "imgui_internal.h"

namespace ImGui {

bool DrawVec3Control(const std::string &label, glm::vec3 &values,
                     float resetValue, float columnWidth) {
    bool ret = false;
    ImGuiIO &io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

    float lineHeight =
        GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushFont(boldFont);
    if (ImGui::Button("X", buttonSize)) {
        ret |= true;
        values.x = resetValue;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ret |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Y", buttonSize)) {
        ret |= true;
        values.y = resetValue;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ret |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Z", buttonSize)) {
        ret |= true;
        values.z = resetValue;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ret |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();

    return ret;
}

void DrawTexture(const SD::Texture &texture, const ImVec2 &size,
                 const ImVec2 &uv0, const ImVec2 &uv1) {
    ImGui::Image((void *)(intptr_t)texture.GetId(), size, uv0, uv1);
}

bool DrawTileMap(const SD::TileMap &tilemap, std::array<glm::vec2, 2> &uvs) {
    auto texture = tilemap.GetTexture();
    bool selected = false;
    if (texture) {
        ImVec2 wsize = ImGui::GetContentRegionAvail();
        float aspect = wsize.x / texture->GetWidth();
        wsize.y = texture->GetHeight() * aspect;
        glm::vec2 tile_size = glm::vec2(tilemap.GetTileSize()) * aspect;

        int cols = std::floor(wsize.x / tile_size.x);
        int rows = std::floor(wsize.y / tile_size.y);
        ImDrawList *DrawList = ImGui::GetWindowDrawList();
        ImGuiWindow *window = ImGui::GetCurrentWindow();

        ImRect bb(window->DC.CursorPos,
                  ImVec2(window->DC.CursorPos.x + cols * tile_size.x,
                         window->DC.CursorPos.y + rows * tile_size.y));
        ImGui::DrawTexture(*texture, wsize);
        for (int i = 0; i <= cols; ++i) {
            DrawList->AddLine(ImVec2(bb.Min.x + i * tile_size.x, bb.Min.y),
                              ImVec2(bb.Min.x + i * tile_size.x, bb.Max.y),
                              ImGui::GetColorU32(ImGuiCol_TextDisabled));
        }
        for (int i = 0; i <= rows; ++i) {
            DrawList->AddLine(ImVec2(bb.Min.x, bb.Min.y + i * tile_size.y),
                              ImVec2(bb.Max.x, bb.Min.y + i * tile_size.y),
                              ImGui::GetColorU32(ImGuiCol_TextDisabled));
        }
        auto [left, top] = bb.GetTL();
        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0)) {
            auto [mouse_x, mouse_y] = ImGui::GetMousePos();
            auto image_pos = glm::vec2(mouse_x - left, mouse_y - top) / aspect;
            if (image_pos.x >= 0 && image_pos.y >= 0 &&
                image_pos.x < texture->GetWidth() &&
                image_pos.y < texture->GetHeight()) {
                uvs = tilemap.GetTileUVs(image_pos);
                selected = true;
            }
        }
        ImRect active_grid(
            ImVec2(left + uvs[0].x * wsize.x, top + uvs[0].y * wsize.y),
            ImVec2(left + uvs[1].x * wsize.x, top + uvs[1].y * wsize.y));
        if (bb.Contains(active_grid)) {
            const ImU32 COLOR = 0xff00ff00;
            const float THICKNESS = 2.f;
            DrawList->AddQuad(active_grid.GetTL(), active_grid.GetTR(),
                              active_grid.GetBR(), active_grid.GetBL(), COLOR,
                              THICKNESS);
        }
    }
    return selected;
}

}  // namespace ImGui
