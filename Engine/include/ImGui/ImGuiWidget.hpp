#ifndef SD_IMGUI_WIDGET_HPP
#define SD_IMGUI_WIDGET_HPP

#include "Utility/Base.hpp"
#include "ImGui/Export.hpp"
#include "Graphics/Texture.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include <glm/glm.hpp>
#include <string>

namespace ImGui {

bool IMGUI_API DrawVec3Control(const std::string &label, glm::vec3 &values,
                               float resetValue = 0.0f,
                               float columnWidth = 100.0f);

void IMGUI_API DrawTexture(const SD::Texture &texture, const ImVec2 &size,
                           const ImVec2 &uv0 = ImVec2(0, 0),
                           const ImVec2 &uv1 = ImVec2(1, 1));

bool IMGUI_API DrawTileTexture(const SD::Texture &texture,
                               std::array<glm::vec2, 2> &uvs,
                               glm::ivec2 &selected_cnt, glm::ivec2 &pivot);

bool IMGUI_API BeginCenterPopupModal(const char *name, bool *p_open = nullptr,
                                     ImGuiWindowFlags flags = 0);
}  // namespace ImGui

#endif
