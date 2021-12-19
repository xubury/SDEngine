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

void IMGUI_API DrawTexture(const SD::Texture &texture, const ImVec2 &size);

}  // namespace ImGui

#endif
