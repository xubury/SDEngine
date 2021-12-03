#ifndef SD_COMPONENT_HPP
#define SD_COMPONENT_HPP

#include "Utility/Base.hpp"
#include "ECS/Export.hpp"
#include "Utility/Serialize.hpp"
#include "Utility/Transform.hpp"
#include "Utility/Math.hpp"
#include "Utility/ResourceId.hpp"
#include "Asset/Model.hpp"
#include "Asset/Font.hpp"
#include "Renderer/Terrain.hpp"
#include "Renderer/Light.hpp"
#include "Renderer/Skybox.hpp"

#include "entt/entt.hpp"

namespace SD {

struct SD_ECS_API IdComponent {
    ResourceId id;

    SERIALIZE(id)
};

struct SD_ECS_API EntityDataComponent {
    std::set<entt::entity> children;
    entt::entity parent;
    EntityDataComponent() : parent(entt::null) {}

    SERIALIZE(parent, children)
};

struct SD_ECS_API TagComponent {
    std::string tag;
    TagComponent() = default;
    TagComponent(const std::string& tag) : tag(tag) {}

    SERIALIZE(tag)
};

struct SD_ECS_API TransformComponent {
    Transform transform;

    SERIALIZE(transform)
};

struct SD_ECS_API ModelComponent {
    ResourceId id;
    glm::vec3 color;
    ModelComponent() = default;

    SERIALIZE(id, color)
};

struct SD_ECS_API TerrainComponent {
    Terrain terrain;
    TerrainComponent() : terrain(1, 10) {}
};

struct SD_ECS_API LightComponent {
    Light light;

    SERIALIZE(light)
};

struct SD_ECS_API TextComponent {
    ResourceId id;
    uint8_t pixel_size = 20;
    glm::vec4 color = glm::vec4(1);
    std::string text;

    SERIALIZE(id, pixel_size, color, text)
};

struct SD_ECS_API CameraComponent {
    Camera camera;
    CameraComponent() = default;
    CameraComponent(CameraType type, float fov, float width, float height,
                    float near_z, float far_z)
        : camera(type, fov, width, height, near_z, far_z) {}

    SERIALIZE(camera)
};

struct SD_ECS_API SkyboxComponent {
    std::array<ResourceId, 6> id;

    Skybox skybox;

    SERIALIZE(id)
};

}  // namespace SD

#endif /* SD_COMPONENT_HPP */
