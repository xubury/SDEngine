#include "ECS/Scene.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Component.hpp"
#include "Utility/Serialize.hpp"
#include "Asset/Asset.hpp"
#include <stack>

namespace SD {

Scene::Scene() {}

Entity Scene::CreateEntity(const std::string &name) {
    Entity entity(create(), this);
    entity.AddComponent<IdComponent>();
    entity.AddComponent<TagComponent>(name);
    entity.AddComponent<TransformComponent>();
    entity.AddComponent<EntityDataComponent>();
    return entity;
}

void Scene::DestroyEntity(Entity &entity, bool is_root) {
    auto &data = entity.GetComponent<EntityDataComponent>();
    for (entt::entity entity_id : data.children) {
        Entity child(entity_id, this);
        DestroyEntity(child, false);
    }
    Entity parent(data.parent, this);
    if (is_root && parent) {
        RemoveChildFromEntity(parent, entity);
    }
    destroy(entity);
}

void Scene::AddChildToEntity(Entity &parent, Entity &child) {
    if (parent == child) return;
    auto &parent_data = parent.GetComponent<EntityDataComponent>();
    auto &child_data = child.GetComponent<EntityDataComponent>();
    if (parent_data.parent == child || child_data.parent == parent) return;

    Transform *child_transform =
        &child.GetComponent<TransformComponent>().transform;
    Transform *parent_transform =
        &parent.GetComponent<TransformComponent>().transform;

    Entity old_parent(child_data.parent, this);
    if (old_parent) {
        RemoveChildFromEntity(old_parent, child);
    }
    parent_transform->AddChild(child_transform);
    parent_data.children.emplace(child);
    child_data.parent = parent;
}

void Scene::RemoveChildFromEntity(Entity &parent, Entity &child) {
    auto &children = parent.GetComponent<EntityDataComponent>().children;
    if (children.find(child) != children.end()) {
        children.erase(child);
        Transform *childTransform =
            &child.GetComponent<TransformComponent>().transform;
        parent.GetComponent<TransformComponent>().transform.RemoveChild(
            childTransform);
    }
    child.GetComponent<EntityDataComponent>().parent = Entity();
}

void Scene::Refresh() {
    auto ecs_data = view<EntityDataComponent>();

    for (auto entity_id : ecs_data) {
        Entity entity(entity_id, this);
        RefreshEntityChildTranforms(entity);
        RefreshLight(entity);
    }
}

void Scene::Load(const std::string &filePath) {
    clear();
    std::ifstream is(filePath, std::ios::binary);
    cereal::XMLInputArchive archive(is);
    entt::snapshot_loader{*this}
        .entities(archive)
        .component<IdComponent, EntityDataComponent, TagComponent,
                   TransformComponent, ModelComponent, LightComponent,
                   TextComponent, SkyboxComponent>(archive);
    Refresh();
}

void Scene::Save(const std::string &filePath) {
    std::ofstream os(filePath, std::ios::binary);
    cereal::XMLOutputArchive archive(os);
    entt::snapshot{*this}
        .entities(archive)
        .component<IdComponent, EntityDataComponent, TagComponent,
                   TransformComponent, ModelComponent, LightComponent,
                   TextComponent, SkyboxComponent>(archive);
}

void Scene::RefreshLight(Entity &entity) {
    if (entity.HasComponent<LightComponent>()) {
        auto &lightComp = entity.GetComponent<LightComponent>();
        if (lightComp.light.IsCastShadow()) {
            lightComp.light.CreateShadowMap();
        }
    }
}

void Scene::RefreshEntityChildTranforms(Entity &entity) {
    EntityDataComponent &data = entity.GetComponent<EntityDataComponent>();
    Transform &transform = entity.GetComponent<TransformComponent>().transform;
    transform.GetChildren().clear();
    Entity parent(data.parent, this);
    if (parent) {
        parent.GetComponent<TransformComponent>().transform.AddChild(
            &transform);
    }

    for (entt::entity childId : data.children) {
        Entity child(childId, this);
        entity.GetComponent<TransformComponent>().transform.AddChild(
            &child.GetComponent<TransformComponent>().transform);
        RefreshEntityChildTranforms(child);
    }
}

}  // namespace SD
