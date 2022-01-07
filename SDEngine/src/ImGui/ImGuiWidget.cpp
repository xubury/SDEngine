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

void DrawTexture(const SD::Texture &texture, const ImVec2 &uv0,
                 const ImVec2 &uv1) {
    ImVec2 wsize = ImGui::GetWindowSize();
    const float width = texture.GetWidth() * std::abs(uv1.x - uv0.x);
    const float height = texture.GetHeight() * std::abs(uv1.y - uv0.y);
    const float ASPECT = height / width;
    wsize.y = wsize.x * ASPECT;

    ImGui::Image((void *)(intptr_t)texture.GetId(), wsize, uv0, uv1);
}

bool DrawTileTexture(const SD::Texture &texture, std::array<glm::vec2, 2> &uvs,
                     glm::ivec2 &selected_cnt, glm::ivec2 &pivot) {
    static glm::ivec2 tile_size(50);
    ImGui::TextUnformatted("Tile Size:");
    ImGui::InputInt2("##Size", &tile_size.x);

    ImVec2 wsize = ImGui::GetWindowSize();
    const float ASPECT = wsize.x / texture.GetWidth();
    wsize.y = texture.GetHeight() * ASPECT;
    const glm::vec2 scaled_tile_size = glm::vec2(tile_size) * ASPECT;

    int cols = std::ceil(wsize.x / scaled_tile_size.x);
    int rows = std::ceil(wsize.y / scaled_tile_size.y);
    ImDrawList *DrawList = ImGui::GetWindowDrawList();
    ImGuiWindow *window = ImGui::GetCurrentWindow();

    ImRect bb(window->DC.CursorPos,
              ImVec2(window->DC.CursorPos.x + cols * scaled_tile_size.x,
                     window->DC.CursorPos.y + rows * scaled_tile_size.y));
    ImGui::DrawTexture(texture);
    for (int i = 0; i <= cols; ++i) {
        DrawList->AddLine(ImVec2(bb.Min.x + i * scaled_tile_size.x, bb.Min.y),
                          ImVec2(bb.Min.x + i * scaled_tile_size.x, bb.Max.y),
                          ImGui::GetColorU32(ImGuiCol_TextDisabled));
    }
    for (int i = 0; i <= rows; ++i) {
        DrawList->AddLine(ImVec2(bb.Min.x, bb.Min.y + i * scaled_tile_size.y),
                          ImVec2(bb.Max.x, bb.Min.y + i * scaled_tile_size.y),
                          ImGui::GetColorU32(ImGuiCol_TextDisabled));
    }
    auto [left, top] = bb.GetTL();
    if (ImGui::IsWindowHovered()) {
        auto [mouse_x, mouse_y] = ImGui::GetMousePos();
        auto image_pos = glm::vec2(mouse_x - left, mouse_y - top) / ASPECT;
        static std::array<glm::vec2, 2> first_uvs;
        if (image_pos.x >= 0 && image_pos.y >= 0 &&
            image_pos.x < texture.GetWidth() &&
            image_pos.y < texture.GetHeight()) {
            if (ImGui::IsMouseClicked(0)) {
                uvs[0] = glm::floor(image_pos / glm::vec2(tile_size));
                uvs[1] = uvs[0] + glm::vec2(1.f);

                uvs[0].x = uvs[0].x * tile_size.x / texture.GetWidth();
                uvs[0].y = uvs[0].y * tile_size.y / texture.GetHeight();

                uvs[1].x = uvs[1].x * tile_size.x / texture.GetWidth();
                uvs[1].y = uvs[1].y * tile_size.y / texture.GetHeight();
                first_uvs = uvs;

                pivot = glm::ivec2(0, 0);

                selected_cnt.x = 1;
                selected_cnt.y = 1;
            } else if (ImGui::IsMouseClicked(1)) {
                pivot = glm::floor(image_pos / glm::vec2(tile_size));
                pivot.x -=
                    std::floor(uvs[0].x * texture.GetWidth() / tile_size.x);
                pivot.y -=
                    std::floor(uvs[0].y * texture.GetHeight() / tile_size.y);
            }
            if (ImGui::IsMouseDown(0)) {
                uvs[0] = glm::floor(image_pos / glm::vec2(tile_size));
                uvs[1] = uvs[0] + glm::vec2(1.f);

                uvs[0].x = uvs[0].x * tile_size.x / texture.GetWidth();
                uvs[0].y = uvs[0].y * tile_size.y / texture.GetHeight();

                uvs[1].x = uvs[1].x * tile_size.x / texture.GetWidth();
                uvs[1].y = uvs[1].y * tile_size.y / texture.GetHeight();

                if (uvs[1].x > first_uvs[0].x) {
                    uvs[0].x = first_uvs[0].x;
                } else {
                    uvs[1].x = first_uvs[1].x;
                }
                if (uvs[1].y > first_uvs[0].y) {
                    uvs[0].y = first_uvs[0].y;
                } else {
                    uvs[1].y = first_uvs[1].y;
                }
                selected_cnt =
                    glm::round((uvs[1] - uvs[0]) * glm::vec2(cols, rows));
            }
        }
    }
    const ImRect active_grid(
        ImVec2(left + uvs[0].x * wsize.x, top + uvs[0].y * wsize.y),
        ImVec2(left + uvs[1].x * wsize.x, top + uvs[1].y * wsize.y));
    if (bb.Contains(active_grid)) {
        const ImU32 GRID_COLOR = 0xff00ff00;
        const ImU32 PIVOT_COLOR = 0x770000ff;
        const float THICKNESS = 2.f;
        const float PIVOT_SIZE = 0.8f;
        DrawList->AddQuad(active_grid.GetTL(), active_grid.GetTR(),
                          active_grid.GetBR(), active_grid.GetBL(), GRID_COLOR,
                          THICKNESS);
        const ImRect pivot_grid(
            ImVec2((pivot.x + 1 - PIVOT_SIZE) * scaled_tile_size.x +
                       active_grid.GetTL().x,
                   (pivot.y + 1 - PIVOT_SIZE) * scaled_tile_size.y +
                       active_grid.GetTL().y),
            ImVec2((pivot.x + PIVOT_SIZE) * scaled_tile_size.x +
                       active_grid.GetTL().x,
                   (pivot.y + PIVOT_SIZE) * scaled_tile_size.y +
                       active_grid.GetTL().y));
        DrawList->AddQuadFilled(pivot_grid.GetTL(), pivot_grid.GetTR(),
                                pivot_grid.GetBR(), pivot_grid.GetBL(),
                                PIVOT_COLOR);
    }
    return selected_cnt.x > 0 && selected_cnt.y > 0;
}

bool BeginCenterPopupModal(const char *name, bool *p_open,
                           ImGuiWindowFlags flags) {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                            ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    return ImGui::BeginPopupModal(
        name, p_open,
        flags | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
}

}  // namespace ImGui
