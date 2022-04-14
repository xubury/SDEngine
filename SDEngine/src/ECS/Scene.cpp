#include "ECS/Scene.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Component.hpp"
#include "Utility/Serialize.hpp"

namespace SD {

Scene::Scene()
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

Entity Scene::CreateModelEntity(const Model &model, const ModelNode *node)

{
    std::string name = node->GetName();
    Entity entity = CreateEntity(name);
    auto &meshes = node->GetMeshes();
    auto &materials = node->GetMaterials();
    for (size_t i = 0; i < meshes.size(); ++i) {
        Entity child = CreateEntity(name + "_" + std::to_string(i));
        entity.AddChild(child);
        auto &mc = child.AddComponent<MeshComponent>();
        mc.model_id = model.Id();
        mc.mesh_index = meshes[i];
        mc.material = model.GetMaterial(materials[i]);
        auto &tc = child.GetComponent<TransformComponent>();
        tc.SetLocalTransform(node->GetTransform());
    }
    for (auto &child : node->GetChildren()) {
        Entity child_entity = CreateModelEntity(model, child);
        entity.AddChild(child_entity);
    }
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

void Scene::Serialize(cereal::PortableBinaryOutputArchive &archive)
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
