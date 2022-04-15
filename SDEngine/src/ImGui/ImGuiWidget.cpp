#include "ImGui/ImGuiWidget.hpp"
#include "Graphics/Model.hpp"
#include "Graphics/Font.hpp"

namespace ImGui {

bool DrawVec3Control(const std::string &label, SD::Vector3f &values,
                     float resetValue, float columnWidth)
{
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
                 const ImVec2 &uv1)
{
    ImVec2 wsize = ImGui::GetContentRegionAvail();
    const float width = texture.GetWidth() * std::abs(uv1.x - uv0.x);
    const float height = texture.GetHeight() * std::abs(uv1.y - uv0.y);
    const float ASPECT = height / width;
    wsize.y = wsize.x * ASPECT;

    ImGui::Image((void *)(intptr_t)texture.Handle(), wsize, uv0, uv1);
}

void DrawTileTexture(const SD::Texture &texture, SD::Vector2i &tile_size,
                     std::array<SD::Vector2f, 2> &uvs,
                     SD::Vector2i *selected_cnt, SD::Vector2i *pivot)
{
    ImGui::PushID(&texture);
    ImGui::InputInt2("Tile Size", &tile_size.x);
    tile_size.x = std::clamp(tile_size.x, 1, texture.GetWidth());
    tile_size.y = std::clamp(tile_size.y, 1, texture.GetHeight());

    ImVec2 wsize = ImGui::GetWindowSize();
    const float aspect = wsize.x / texture.GetWidth();
    wsize.y = texture.GetHeight() * aspect;
    const SD::Vector2f scaled_tile_size = SD::Vector2f(tile_size) * aspect;

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
        auto image_pos = SD::Vector2f(mouse_x - left, mouse_y - top) / aspect;
        static std::array<SD::Vector2f, 2> first_uvs;
        if (image_pos.x >= 0 && image_pos.y >= 0 &&
            image_pos.x < texture.GetWidth() &&
            image_pos.y < texture.GetHeight()) {
            if (ImGui::IsMouseClicked(0)) {
                uvs[0] = glm::floor(image_pos / SD::Vector2f(tile_size));
                uvs[1] = uvs[0] + SD::Vector2f(1.f);

                uvs[0].x = uvs[0].x * tile_size.x / texture.GetWidth();
                uvs[0].y = uvs[0].y * tile_size.y / texture.GetHeight();

                uvs[1].x = uvs[1].x * tile_size.x / texture.GetWidth();
                uvs[1].y = uvs[1].y * tile_size.y / texture.GetHeight();
                first_uvs = uvs;

                if (pivot) {
                    *pivot = SD::Vector2i(0, 0);
                }

                if (selected_cnt) {
                    selected_cnt->x = 1;
                    selected_cnt->y = 1;
                }
            }
            else if (ImGui::IsMouseClicked(1)) {
                if (pivot) {
                    *pivot = glm::floor(image_pos / SD::Vector2f(tile_size));
                    pivot->x -=
                        std::floor(uvs[0].x * texture.GetWidth() / tile_size.x);
                    pivot->y -= std::floor(uvs[0].y * texture.GetHeight() /
                                           tile_size.y);
                }
            }
            if (ImGui::IsMouseDown(0)) {
                uvs[0] = glm::floor(image_pos / SD::Vector2f(tile_size));
                uvs[1] = uvs[0] + SD::Vector2f(1.f);

                uvs[0].x = uvs[0].x * tile_size.x / texture.GetWidth();
                uvs[0].y = uvs[0].y * tile_size.y / texture.GetHeight();

                uvs[1].x = uvs[1].x * tile_size.x / texture.GetWidth();
                uvs[1].y = uvs[1].y * tile_size.y / texture.GetHeight();

                if (uvs[1].x > first_uvs[0].x) {
                    uvs[0].x = first_uvs[0].x;
                }
                else {
                    uvs[1].x = first_uvs[1].x;
                }
                if (uvs[1].y > first_uvs[0].y) {
                    uvs[0].y = first_uvs[0].y;
                }
                else {
                    uvs[1].y = first_uvs[1].y;
                }
                if (selected_cnt) {
                    *selected_cnt = glm::round((uvs[1] - uvs[0]) *
                                               SD::Vector2f(cols, rows));
                }
            }
        }
    }
    const ImRect active_grid(
        ImVec2(left + uvs[0].x * wsize.x, top + uvs[0].y * wsize.y),
        ImVec2(left + uvs[1].x * wsize.x, top + uvs[1].y * wsize.y));
    if (bb.Contains(active_grid)) {
        const ImU32 grid_color = 0xff00ff00;
        const ImU32 pivot_color = 0x770000ff;
        const float thickness = 2.f;
        const float pivot_size = 0.8f;
        DrawList->AddQuad(active_grid.GetTL(), active_grid.GetTR(),
                          active_grid.GetBR(), active_grid.GetBL(), grid_color,
                          thickness);
        if (pivot) {
            const ImRect pivot_grid(
                ImVec2((pivot->x + 1 - pivot_size) * scaled_tile_size.x +
                           active_grid.GetTL().x,
                       (pivot->y + 1 - pivot_size) * scaled_tile_size.y +
                           active_grid.GetTL().y),
                ImVec2((pivot->x + pivot_size) * scaled_tile_size.x +
                           active_grid.GetTL().x,
                       (pivot->y + pivot_size) * scaled_tile_size.y +
                           active_grid.GetTL().y));
            DrawList->AddQuadFilled(pivot_grid.GetTL(), pivot_grid.GetTR(),
                                    pivot_grid.GetBR(), pivot_grid.GetBL(),
                                    pivot_color);
        }
    }
    ImGui::PopID();
}

bool BeginCenterPopupModal(const char *name, bool *p_open,
                           ImGuiWindowFlags flags)
{
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                            ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    return ImGui::BeginPopupModal(name, p_open,
                                  flags | ImGuiWindowFlags_AlwaysAutoResize);
}

bool DrawTextureAssetSelection(const SD::ResourceCache<SD::Texture> &,
                               SD::ResourceId *)
{
    bool ret = false;
    // if (!resource.Empty<SD::Texture>()) {
    //     ImGuiStyle &style = ImGui::GetStyle();
    //     const auto &cache = resource.GetCaches<SD::Texture>();
    //     ImVec2 combo_pos = ImGui::GetCursorScreenPos();
    //     if (ImGui::BeginCombo("##TEXTURE_ASSETS_COMBO", "")) {
    //         for (auto &[rid, asset] : cache) {
    //             ImGui::PushID(rid);
    //             auto texture = resource.GetResource<SD::Texture>(rid);
    //             const bool is_selected = (*id == rid);
    //             if (ImGui::Selectable("", is_selected)) {
    //                 *id = rid;
    //                 ret = true;
    //             }

    //             ImGui::SameLine();
    //             float h = ImGui::GetTextLineHeight();
    //             ImGui::Image(
    //                 (void *)(intptr_t)texture->Handle(),
    //                 ImVec2(h * texture->GetWidth() / texture->GetHeight(),
    //                 h));

    //             ImGui::SameLine();
    //             ImGui::TextUnformatted("Texture Name");

    //             ImGui::PopID();
    //         }
    //         ImGui::EndCombo();
    //     }
    //     ImVec2 old_pos = ImGui::GetCursorScreenPos();
    //     ImGui::SetCursorScreenPos(ImVec2(combo_pos.x + style.FramePadding.x,
    //                                      combo_pos.y +
    //                                      style.FramePadding.y));
    //     if (resource.Exist<SD::Texture>(*id)) {
    //         float h = ImGui::GetTextLineHeight();
    //         auto texture = resource.GetResource<SD::Texture>(*id);
    //         ImGui::Image(
    //             (void *)(intptr_t)texture->Handle(),
    //             ImVec2(h * texture->GetWidth() / texture->GetHeight(), h));
    //         ImGui::SameLine();
    //         ImGui::TextUnformatted("Texture Name");
    //     }
    //     else {
    //         ImGui::TextUnformatted("None");
    //     }
    //     ImGui::SetCursorScreenPos(old_pos);
    // }
    return ret;
}

// bool DrawModelAssetSelection(SD::ResourceId *id)
// {
//     bool ret = false;
//     auto &resource = SD::ResourceManager::Get();
//     if (!resource.Empty<SD::Model>()) {
//         const auto &cache = resource.GetCaches<SD::Model>();
//         const std::string item =
//             resource.Exist<SD::Model>(*id) ? std::to_string(*id) : "None";
//         if (ImGui::BeginCombo("##MODEL_ASSETS_COMBO", item.c_str())) {
//             for (auto &[rid, asset] : cache) {
//                 const bool is_selected = (*id == rid);
//                 if (ImGui::Selectable(std::to_string(rid).c_str(),
//                                       is_selected)) {
//                     *id = rid;
//                     ret = true;
//                 }
//                 if (is_selected) ImGui::SetItemDefaultFocus();
//             }
//             ImGui::EndCombo();
//         }
//     }
//     return ret;
// }

// bool DrawFontAssetSelection(SD::ResourceId *id)
// {
//     const char32_t preview_char = 65;
//     bool ret = false;
//     auto &resource = SD::ResourceManager::Get();
//     if (!resource.Empty<SD::Font>()) {
//         ImGuiStyle &style = ImGui::GetStyle();
//         const auto &cache = resource.GetCaches<SD::Font>();
//         ImVec2 combo_pos = ImGui::GetCursorScreenPos();
//         if (ImGui::BeginCombo("##TEXTURE_ASSETS_COMBO", "")) {
//             for (auto &[rid, asset] : cache) {
//                 ImGui::PushID(rid);
//                 auto &character =
//                     resource.GetResource<SD::Font>(rid)->GetCharacter(
//                         preview_char);
//                 auto &texture = character.glyph;
//                 auto &uv = character.uv;
//                 const bool is_selected = (*id == rid);
//                 if (ImGui::Selectable("", is_selected)) {
//                     *id = rid;
//                     ret = true;
//                 }

//                 ImGui::SameLine();
//                 float h = ImGui::GetTextLineHeight();
//                 ImGui::Image(
//                     (void *)(intptr_t)texture->Handle(),
//                     ImVec2(h * texture->GetWidth() / texture->GetHeight(),
//                     h), ImVec2(uv[0].x, uv[0].y), ImVec2(uv[1].x, uv[1].y));

//                 ImGui::SameLine();
//                 ImGui::TextUnformatted(std::to_string(rid).c_str());

//                 ImGui::PopID();
//             }
//             ImGui::EndCombo();
//         }
//         ImVec2 old_pos = ImGui::GetCursorScreenPos();
//         ImGui::SetCursorScreenPos(ImVec2(combo_pos.x + style.FramePadding.x,
//                                          combo_pos.y +
//                                          style.FramePadding.y));
//         if (resource.Exist<SD::Font>(*id)) {
//             float h = ImGui::GetTextLineHeight();
//             auto &character =
//                 resource.GetResource<SD::Font>(*id)->GetCharacter(preview_char);
//             auto &texture = character.glyph;
//             auto &uv = character.uv;
//             ImGui::Image(
//                 (void *)(intptr_t)texture->Handle(),
//                 ImVec2(h * texture->GetWidth() / texture->GetHeight(), h),
//                 ImVec2(uv[0].x, uv[0].y), ImVec2(uv[1].x, uv[1].y));
//             ImGui::SameLine();
//             ImGui::TextUnformatted(std::to_string(*id).c_str());
//         }
//         else {
//             ImGui::TextUnformatted("None");
//         }
//         ImGui::SetCursorScreenPos(old_pos);
//     }
//     return ret;
// }

}  // namespace ImGui
