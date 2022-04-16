#ifndef SD_SCENE_HPP
#define SD_SCENE_HPP

#include "Utility/Base.hpp"
#include "Utility/Serialize.hpp"
#include "ECS/Export.hpp"
#include "Graphics/Model.hpp"
#include "Resource/ResourceRegistry.hpp"
#include "Resource/ResourceCache.hpp"

#include "entt/entt.hpp"

namespace SD {

using EntityId = entt::entity;
using EntityIdType = entt::id_type;
class Entity;

using ComponentSerializeFunction = entt::delegate<void(
    entt::snapshot &, cereal::PortableBinaryOutputArchive &)>;

using ComponentDeserializeFunction = entt::delegate<void(
    entt::snapshot_loader &, cereal::PortableBinaryInputArchive &)>;

template <typename T>
void OnComponentAdded(entt::registry &, entt::entity)
{
}

class SD_ECS_API Scene {
   public:
    Scene();
    ~Scene() = default;

    Entity CreateEntity(const std::string &name);
    Entity CreateModelEntity(const ResourceRegistry::ModelHandle &model,
                             const ModelNode *node);

    void Serialize(cereal::PortableBinaryOutputArchive &archive) const;

    void Deserialize(cereal::PortableBinaryInputArchive &archive);

    Entity CloneEntity(EntityId from);

    // Registering a component enables serialization as well as duplication
    // functionality in & out editor.
    template <typename T>
    void RegisterComponent()
    {
        auto id = entt::type_hash<T>::value();
        m_serialize_functions[id]
            .first.template connect<&Scene::SerializeComponent<T>>();
        m_serialize_functions[id]
            .second.template connect<&Scene::DeserializeComponent<T>>();
        m_entity_reg.on_construct<T>().template connect<&OnComponentAdded<T>>();
    }
    entt::registry &GetEntityRegistry() { return m_entity_reg; }
    const entt::registry &GetEntityRegistry() const { return m_entity_reg; }

    ResourceRegistry &GetResourceRegistry() { return m_resource_reg; }
    const ResourceRegistry &GetResourceRegistry() const
    {
        return m_resource_reg;
    }

   private:
    template <typename T>
    static void SerializeComponent(entt::snapshot &snapshot,
                                   cereal::PortableBinaryOutputArchive &archive)
    {
        snapshot.component<T>(archive);
    }

    template <typename T>
    static void DeserializeComponent(
        entt::snapshot_loader &loader,
        cereal::PortableBinaryInputArchive &archive)
    {
        loader.component<T>(archive);
    }

    template <typename T>
    void CloneComponent(EntityId from, EntityId to)
    {
        T component = m_entity_reg.get<T>(from);
        m_entity_reg.emplace<T>(to, component);
    }

    std::unordered_map<EntityIdType, std::pair<ComponentSerializeFunction,
                                               ComponentDeserializeFunction>>
        m_serialize_functions;

    ResourceRegistry m_resource_reg;
    entt::registry m_entity_reg;

    friend class Entity;
};

}  // namespace SD

#endif /* SD_SCENE_HPP */
