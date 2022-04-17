#ifndef SD_IMGUI_WIDGET_HPP
#define SD_IMGUI_WIDGET_HPP

#include "Utility/Base.hpp"
#include "Utility/Math.hpp"
#include "ImGui/Export.hpp"
#include "Graphics/Texture.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include <string>

namespace ImGui {

bool IMGUI_API DrawVec3Control(const std::string &label, SD::Vector3f &values,
                               float resetValue = 0.0f,
                               float columnWidth = 100.0f);

void IMGUI_API DrawTexture(const SD::Texture &texture,
                           const ImVec2 &uv0 = ImVec2(0, 0),
                           const ImVec2 &uv1 = ImVec2(1, 1));

void IMGUI_API DrawTileTexture(const SD::Texture &texture,
                               SD::Vector2i &tile_size,
                               std::array<SD::Vector2f, 2> &uvs,
                               SD::Vector2i *selected_cnt = nullptr,
                               SD::Vector2i *pivot = nullptr);

bool IMGUI_API BeginCenterPopupModal(const char *name, bool *p_open = nullptr,
                                     ImGuiWindowFlags flags = 0);

// bool IMGUI_API DrawTextureAssetSelection(
//     const SD::ResourceRegistry::TextureCache &cache, SD::ResourceId *id);

// bool IMGUI_API DrawModelAssetSelection(SD::ResourceId *id);

// bool IMGUI_API DrawFontAssetSelection(SD::ResourceId *id);

}  // namespace ImGui

#endif
