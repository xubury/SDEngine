#ifndef SD_COMPONENT_HPP
#define SD_COMPONENT_HPP

#include "Utils/Export.hpp"
#include "Utils/Transform.hpp"
#include "Utils/Math.hpp"
#include "ECS/Entity.hpp"
#include "Graphics/Model.hpp"
#include "Graphics/Mesh.hpp"
#include "Graphics/Terrain.hpp"
#include "Graphics/Light.hpp"

namespace sd {

struct SD_API EntityDataComponent {
    std::set<entt::entity> m_children;
    entt::entity m_parent;
    EntityDataComponent() : m_parent(entt::null) {}

    template <class Archive>
    void serialize(Archive& archive) {
        archive(m_parent, m_children);
    }
};

struct SD_API TagComponent {
    std::string tag;
    TagComponent() = default;
    TagComponent(const std::string& tag) : tag(tag) {}

    template <typename Archive>
    void serialize(Archive& archive) {
        archive(tag);
    }
};

struct SD_API TransformComponent {
    Transform transform;

    template <typename Archive>
    void serialize(Archive& archive) {
        archive(transform);
    }
};

struct SD_API ModelComponent {
    std::string path;
    Ref<Model> model;
    glm::vec3 color;
    ModelComponent() : model(createRef<Model>()), color(0.f) {}

    template <typename Archive>
    void serialize(Archive& archive) {
        archive(path, color);
    }
};

struct SD_API TerrainComponent {
    Terrain terrain;
    TerrainComponent();
};

struct SD_API LightComponent {
    Light light;

    template <typename Archive>
    void serialize(Archive& archive) {
        archive(light);
    }
};

}  // namespace sd

#endif /* SD_COMPONENT_HPP */
