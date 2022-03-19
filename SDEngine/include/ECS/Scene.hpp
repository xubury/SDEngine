#ifndef SD_SCENE_HPP
#define SD_SCENE_HPP

#include "Utility/Base.hpp"
#include "Utility/Serialize.hpp"
#include "ECS/Export.hpp"
#include "ECS/Component.hpp"

#include "entt/entt.hpp"
#define ENTT_USE_ATOMIC
#include "entt/config/config.h"
#include "entt/entity/snapshot.hpp"
#include "entt/entity/registry.hpp"
#include "entt/entity/entity.hpp"

namespace SD {

class Entity;

using ComponentSerializeFunction = entt::delegate<void(
    entt::snapshot &, cereal::PortableBinaryOutputArchive &)>;

using ComponentDeserializeFunction = entt::delegate<void(
    entt::snapshot_loader &, cereal::PortableBinaryInputArchive &)>;

class SD_ECS_API Scene : public entt::registry {
   public:
    Scene();
    ~Scene() = default;

    Entity CreateEntity(const std::string &name);

    void Serialize(cereal::PortableBinaryOutputArchive &archive);

    void Deserialize(cereal::PortableBinaryInputArchive &archive);

    void OnEditor(float dt, Camera *camera);

    void OnRuntime(float dt);
    Camera *GetCamera();

    Entity CloneEntity(EntityId from);

    // Registering a component enables serialization as well as duplication
    // functionality in & out editor.
    template <typename T>
    void RegisterComponent()
    {
        auto id = entt::type_hash<T>::value();
        m_serialize_functions[id]
            .first.template connect<&Scene::SerializeComponent<T>>(this);
        m_serialize_functions[id]
            .second.template connect<&Scene::DeserializeComponent<T>>(this);
        on_construct<T>().template connect<&OnComponentAdded<T>>();
    }

   private:
    template <typename T>
    void SerializeComponent(entt::snapshot &snapshot,
                            cereal::PortableBinaryOutputArchive &archive)
    {
        snapshot.component<T>(archive);
    }

    template <typename T>
    void DeserializeComponent(entt::snapshot_loader &loader,
                              cereal::PortableBinaryInputArchive &archive)
    {
        loader.component<T>(archive);
    }

    template <typename T>
    void CloneComponent(EntityId from, EntityId to)
    {
        T component = get<T>(from);
        emplace<T>(to, component);
    }

    Camera *m_camera;
    EntityId m_selected_entity;
    std::unordered_map<EntityIdType, std::pair<ComponentSerializeFunction,
                                               ComponentDeserializeFunction>>
        m_serialize_functions;
};

}  // namespace SD

#endif /* SD_SCENE_HPP */
