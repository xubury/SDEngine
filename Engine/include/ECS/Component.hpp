#ifndef SD_COMPONENT_HPP
#define SD_COMPONENT_HPP

#include "entt/entt.hpp"
#include "Utility/Export.hpp"
#include "Utility/Transform.hpp"
#include "Utility/Math.hpp"
#include "Renderer/Model.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/Terrain.hpp"
#include "Renderer/Light.hpp"
#include "Renderer/Font.hpp"

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

struct SD_API TextComponent {
    std::string fontPath;
    int pixelSize = 10;
    glm::vec4 color = glm::vec4(1);
    std::wstring text;

    template <typename Archive>
    void serialize(Archive& archive) {
        archive(fontPath, pixelSize, color, text);
    }
};

}  // namespace sd

#endif /* SD_COMPONENT_HPP */
