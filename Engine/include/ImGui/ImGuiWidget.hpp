#ifndef SD_IMGUI_WIDGET_HPP
#define SD_IMGUI_WIDGET_HPP

#include "Utility/Base.hpp"
#include "Graphics/Texture.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include <glm/glm.hpp>
#include <string>

namespace ImGui {

bool SD_API DrawVec3Control(const std::string &label, glm::vec3 &values,
                            float resetValue = 0.0f,
                            float columnWidth = 100.0f);

void SD_API DrawTexture(const SD::Texture &texture, const ImVec2 &size,
                        const ImVec2 &uv0 = ImVec2(0, 0),
                        const ImVec2 &uv1 = ImVec2(1, 1));

}  // namespace ImGui

#endif
