#ifndef SD_COMPONENT_HPP
#define SD_COMPONENT_HPP

#include "Utility/Base.hpp"
#include "ECS/Export.hpp"
#include "Utility/Serialize.hpp"
#include "Utility/Transform.hpp"
#include "Utility/Math.hpp"
#include "Utility/ResourceId.hpp"
#include "Renderer/Model.hpp"
#include "Renderer/Font.hpp"
#include "Renderer/Terrain.hpp"
#include "Renderer/Light.hpp"
#include "Renderer/Skybox.hpp"
#include "Animation/Animation.hpp"

#include "entt/entt.hpp"

namespace SD {

struct SD_ECS_API IdComponent {
    ResourceId id;

    SERIALIZE(id)
};

struct SD_ECS_API TagComponent {
    std::string tag;
    TagComponent() = default;
    TagComponent(const std::string& tag) : tag(tag) {}

    SERIALIZE(tag)
};

struct SD_ECS_API TransformComponent {
    void OnComponentAdded(entt::registry& reg, entt::entity ent) {
        auto& data = reg.get<TransformComponent>(ent);
        data.ecs = &reg;
    }
    std::set<entt::entity> children;
    entt::entity parent;
    entt::registry* ecs{nullptr};

    TransformComponent();

    void SetLocalPosition(const glm::vec3& position);
    void SetLocalRotation(const glm::quat& rotation);
    void SetLocalScale(const glm::vec3& scale);
    void SetLocalTransform(const glm::mat4& transform);

    glm::vec3 GetLocalPosition() const;
    glm::quat GetLocalRotation() const;
    glm::vec3 GetLocalScale() const;
    const Transform& GetLocalTransform() const;

    void SetWorldPosition(const glm::vec3& position);
    void SetWorldRotation(const glm::quat& rotation);
    void SetWorldScale(const glm::vec3& scale);
    void SetWorldTransform(const glm::mat4& transform);

    glm::vec3 GetWorldPosition() const;
    glm::quat GetWorldRotation() const;
    glm::vec3 GetWorldScale() const;
    const Transform& GetWorldTransform() const;

    glm::vec3 GetLocalRight() const;
    glm::vec3 GetLocalUp() const;
    glm::vec3 GetLocalFront() const;

    glm::vec3 GetWorldRight() const;
    glm::vec3 GetWorldUp() const;
    glm::vec3 GetWorldFront() const;

    void UpdateGlobalPosition();
    void UpdateGlobalRotation();
    void UpdateGlobalScale();

    void UpdateLocalPosition();
    void UpdateLocalRotation();
    void UpdateLocalScale();

    SERIALIZE(parent, children, m_local_transform, m_world_transform)
   private:
    Transform m_world_transform;
    Transform m_local_transform;
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
    glm::vec4 color = glm::vec4(1);
    std::string text;

    SERIALIZE(id, color, text)
};

struct SD_ECS_API CameraComponent {
    Camera camera;
    CameraComponent() = default;
    CameraComponent(CameraType type, float fov, float width, float height,
                    float near_z, float far_z)
        : camera(type, fov, width, height, near_z, far_z) {}

    SERIALIZE(camera)
};

struct SD_ECS_API Frame {
    ResourceId id;
    std::array<glm::vec2, 2> uvs;
    glm::vec2 size;
    int priority;

    SERIALIZE(id, uvs, size, priority)
};

struct SD_ECS_API SpriteComponent {
    ResourceId id;
    std::array<glm::vec2, 2> uvs;
    glm::vec2 size;
    int priority;

    SERIALIZE(id, uvs, size, priority)
};

struct SD_ECS_API SpriteAnimationComponent {
    std::vector<FrameAnimation<Frame>> animations;

    SERIALIZE(animations)
};

template <typename T>
void OnComponentAdded(entt::registry&, entt::entity) {}

template <>
inline void OnComponentAdded<TransformComponent>(entt::registry& reg,
                                                 entt::entity ent) {
    auto& data = reg.get<TransformComponent>(ent);
    data.ecs = &reg;
}

template <>
inline void OnComponentAdded<LightComponent>(entt::registry& reg,
                                             entt::entity ent) {
    auto& lightComp = reg.get<LightComponent>(ent);
    if (lightComp.light.IsCastShadow()) {
        lightComp.light.CreateShadowMap();
    }
}

template <>
inline void OnComponentAdded<SpriteAnimationComponent>(entt::registry& reg,
                                                       entt::entity ent) {
    auto& anim = reg.get<SpriteAnimationComponent>(ent);
    anim.animations.push_back(FrameAnimation<Frame>());
}

}  // namespace SD

#endif /* SD_COMPONENT_HPP */
