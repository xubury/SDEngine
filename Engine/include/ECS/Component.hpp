#ifndef SD_COMPONENT_HPP
#define SD_COMPONENT_HPP

#include "entt/entt.hpp"
#include "Utility/Base.hpp"
#include "Utility/Transform.hpp"
#include "Utility/Math.hpp"
#include "Utility/ResourceId.hpp"
#include "Renderer/Model.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/Terrain.hpp"
#include "Renderer/Light.hpp"
#include "Renderer/Font.hpp"

namespace SD {

struct SD_API IDComponent {
    ResourceId id;

    SERIALIZE(id)
};

struct SD_API EntityDataComponent {
    std::set<entt::entity> children;
    entt::entity parent;
    EntityDataComponent() : parent(entt::null) {}

    SERIALIZE(parent, children)
};

struct SD_API TagComponent {
    std::string tag;
    TagComponent() = default;
    TagComponent(const std::string& tag) : tag(tag) {}

    SERIALIZE(tag)
};

struct SD_API TransformComponent {
    Transform transform;

    SERIALIZE(transform)
};

struct SD_API ModelComponent {
    ResourceId id;
    glm::vec3 color;
    ModelComponent() = default;

    SERIALIZE(id, color)
};

struct SD_API TerrainComponent {
    Terrain terrain;
    TerrainComponent();
};

struct SD_API LightComponent {
    Light light;

    SERIALIZE(light)
};

struct SD_API TextComponent {
    ResourceId id;
    int pixelSize = 10;
    glm::vec4 color = glm::vec4(1);
    std::wstring text;

    SERIALIZE(id, pixelSize, color, text)
};

}  // namespace SD

#endif /* SD_COMPONENT_HPP */
