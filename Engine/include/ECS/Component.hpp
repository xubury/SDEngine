#ifndef SD_COMPONENT_HPP
#define SD_COMPONENT_HPP

#include "Common/Export.hpp"
#include "Utils/Transform.hpp"
#include "ECS/Entity.hpp"
#include "Graphics/Model.hpp"
#include "Graphics/Mesh.hpp"
#include "Graphics/Terrain.hpp"

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

struct SD_API ModelComponent {
    std::string path;
    Ref<Model> model;
    ModelComponent() : path("Empty"), model(sd::createRef<Model>()) {}
};

struct SD_API TerrainComponent {
    Terrain m_terrain;
    TerrainComponent() : m_terrain(1, 10) {}
};

}  // namespace sd

#endif /* SD_COMPONENT_HPP */
