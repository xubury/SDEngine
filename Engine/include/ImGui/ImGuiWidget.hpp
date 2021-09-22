#ifndef SD_IMGUI_WIDGET_HPP
#define SD_IMGUI_WIDGET_HPP

#include <glm/glm.hpp>
#include <string>
#include "Utils/Export.hpp"

namespace ImGui {

void SD_API DrawVec3Control(const std::string &label, glm::vec3 &values,
                            float resetValue = 0.0f,
                            float columnWidth = 100.0f);
}  // namespace sd

#endif
