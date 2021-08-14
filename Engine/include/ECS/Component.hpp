#ifndef SD_COMPONENT_HPP
#define SD_COMPONENT_HPP

#include "Common/Export.hpp"
#include "Utils/Transform.hpp"
#include "ECS/Entity.hpp"
#include "Graphics/Model.hpp"
#include "Graphics/Mesh.hpp"

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

struct SD_API MeshComponent {
    Mesh mesh;
};

struct SD_API ModelComponent {
    std::string path = "Empty";
    Model model;
};

}  // namespace sd

#endif /* SD_COMPONENT_HPP */
