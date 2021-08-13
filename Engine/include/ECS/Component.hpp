#ifndef SD_COMPONENT_HPP
#define SD_COMPONENT_HPP

#include "Common/Export.hpp"
#include "Core/Transform.hpp"
#include "ECS/Entity.hpp"
#include <string>

namespace sd {

struct SD_API TagComponent {
    std::string tag;
    TagComponent() = default;
    TagComponent(const std::string& tag) : tag(tag) {}
};

struct SD_API EntityDataComponent {
    std::set<sd::Entity> m_children;
    sd::Entity m_parent;
};

struct SD_API TransformComponent {
    Transform transform;
};

}  // namespace sd

#endif /* SD_COMPONENT_HPP */
