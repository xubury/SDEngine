#ifndef SD_IMGUI_WIDGET_HPP
#define SD_IMGUI_WIDGET_HPP

#include <glm/glm.hpp>
#include <string>
#include "imgui.h"
#include "Utils/Export.hpp"
#include "Graphics/Texture.hpp"

namespace ImGui {

bool SD_API DrawVec3Control(const std::string &label, glm::vec3 &values,
                            float resetValue = 0.0f,
                            float columnWidth = 100.0f);

void SD_API DrawTexture(const sd::Texture &texture, const ImVec2 &size);

}  // namespace ImGui

#endif
