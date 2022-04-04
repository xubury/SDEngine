#ifndef SD_COMPONENT_HPP
#define SD_COMPONENT_HPP

#include "Utility/Base.hpp"
#include "ECS/Export.hpp"
#include "Utility/Serialize.hpp"
#include "Utility/Transform.hpp"
#include "Utility/Math.hpp"
#include "Utility/ResourceId.hpp"
#include "Graphics/Model.hpp"
#include "Graphics/Font.hpp"
#include "Graphics/Light.hpp"
#include "Graphics/CascadeShadow.hpp"
#include "Animation/Animation.hpp"
#include "Animation/Animator.hpp"

#include "ECS/Scene.hpp"

namespace SD {

struct SD_ECS_API IdComponent {
    ResourceId id;

    SERIALIZE(id)
};

struct SD_ECS_API TagComponent {
    std::string tag;

    SERIALIZE(tag)
};

struct SD_ECS_API TransformComponent {
    std::set<EntityId> children;
    entt::entity parent;
    entt::registry* ecs{nullptr};

    TransformComponent();

    void SetLocalPosition(const Vector3f& position);
    void SetLocalRotation(const Quaternion& rotation);
    void SetLocalScale(const Vector3f& scale);
    void SetLocalTransform(const Matrix4f& transform);

    Vector3f GetLocalPosition() const;
    Quaternion GetLocalRotation() const;
    Vector3f GetLocalScale() const;
    const Transform& GetLocalTransform() const;

    void SetWorldPosition(const Vector3f& position);
    void SetWorldRotation(const Quaternion& rotation);
    void SetWorldScale(const Vector3f& scale);
    void SetWorldTransform(const Matrix4f& transform);

    Vector3f GetWorldPosition() const;
    Quaternion GetWorldRotation() const;
    Vector3f GetWorldScale() const;
    const Transform& GetWorldTransform() const;

    Vector3f GetLocalRight() const;
    Vector3f GetLocalUp() const;
    Vector3f GetLocalFront() const;

    Vector3f GetWorldRight() const;
    Vector3f GetWorldUp() const;
    Vector3f GetWorldFront() const;

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
    ResourceId model_id;

    SERIALIZE(model_id)
};

struct SD_ECS_API LightComponent {
    Light light;
    CascadeShadow shadow;

    SERIALIZE(light, shadow)
};

struct SD_ECS_API TextComponent {
    ResourceId font_id;
    Vector4f color = Vector4f(1);
    std::string text;

    SERIALIZE(font_id, color, text)
};

struct SD_ECS_API CameraComponent {
    Camera camera;
    bool primary{true};
    CameraComponent() = default;
    CameraComponent(CameraType type, float fov, float width, float height,
                    float near_z, float far_z)
        : camera(type, fov, width, height, near_z, far_z)
    {
    }

    SERIALIZE(camera, primary)
};

struct SD_ECS_API SpriteFrame {
    ResourceId texture_id;
    std::array<Vector2f, 2> uvs{Vector2f(0), Vector2f(1)};
    Vector2f size{10.0f};
    int priority;

    SERIALIZE(texture_id, uvs, size, priority)
};

struct SD_ECS_API SpriteComponent {
    SpriteFrame frame;

    SERIALIZE(frame)
};

struct SD_ECS_API SpriteAnimationComponent {
    std::vector<FrameAnimation<SpriteFrame>> animations;
    Animator<FrameAnimation<SpriteFrame>> animator;

    SERIALIZE(animations)
};

template <>
inline void OnComponentAdded<TransformComponent>(entt::registry& reg,
                                                 entt::entity ent)
{
    auto& data = reg.get<TransformComponent>(ent);
    data.ecs = &reg;
}

template <>
inline void OnComponentAdded<LightComponent>(entt::registry& reg,
                                             entt::entity ent)
{
    auto& lightComp = reg.get<LightComponent>(ent);
    if (lightComp.light.IsCastShadow()) {
        lightComp.shadow.CreateShadowMap();
    }
}

template <>
inline void OnComponentAdded<SpriteAnimationComponent>(entt::registry& reg,
                                                       entt::entity ent)
{
    auto& anim = reg.get<SpriteAnimationComponent>(ent);
    anim.animations.push_back(FrameAnimation<SpriteFrame>());
}

}  // namespace SD

#endif /* SD_COMPONENT_HPP */
