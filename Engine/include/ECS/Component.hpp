#ifndef SD_COMPONENT_HPP
#define SD_COMPONENT_HPP

#include "Common/Export.hpp"
#include <string>

namespace sd {

struct SD_API TagComponent {
    std::string tag;
    TagComponent() = default;
    TagComponent(const std::string& tag) : tag(tag) {}
};

}  // namespace sd

#endif /* SD_COMPONENT_HPP */
