#include "ECS/Scene.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Component.hpp"
#include "Utility/Serialize.hpp"

namespace SD {

Scene::Scene(std::string name) : name(std::move(name))
{
    RegisterComponent<IdComponent>();
    RegisterComponent<TagComponent>();
    RegisterComponent<TransformComponent>();
    RegisterComponent<MeshComponent>();
    RegisterComponent<DirectionalLightComponent>();
    RegisterComponent<PointLightComponent>();
    RegisterComponent<TextComponent>();
    RegisterComponent<CameraComponent>();
    RegisterComponent<SpriteComponent>();
    RegisterComponent<SpriteAnimationComponent>();
}

Entity Scene::CreateEntity(const std::string &name)
{
    Entity entity(create(), this);
    entity.AddComponent<IdComponent>();
    entity.AddComponent<TagComponent>(name);
    entity.AddComponent<TransformComponent>();
    return entity;
}

Entity Scene::CloneEntity(EntityId from)
{
    EntityId to = create();
    for (auto &&curr : storage()) {
        if (auto &storage = curr.second; storage.contains(from)) {
            storage.emplace(to, storage.get(from));
        }
    }
    get<IdComponent>(to).id = ResourceId();  // create a new ResourceId;
    get<TransformComponent>(to).children.clear();
    auto &children = get<TransformComponent>(from).children;
    Entity to_entity{to, this};
    for (auto &child : children) {
        Entity entity = CloneEntity(child);
        to_entity.AddChild(entity);
    }
    return to_entity;
}

void Scene::Serialize(cereal::PortableBinaryOutputArchive &archive) const
{
    entt::snapshot loader{*this};
    loader.entities(archive);
    for (auto &func : m_serialize_functions) {
        func.second.first(loader, archive);
    }
}

void Scene::Deserialize(cereal::PortableBinaryInputArchive &archive)
{
    entt::snapshot_loader loader{*this};
    loader.entities(archive);
    for (auto &func : m_serialize_functions) {
        func.second.second(loader, archive);
    }
}

}  // namespace SD
