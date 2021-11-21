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
    entity.AddComponent<IDComponent>();
    entity.AddComponent<TagComponent>(name);
    entity.AddComponent<TransformComponent>();
    entity.AddComponent<EntityDataComponent>();
    return entity;
}

void Scene::DestroyEntity(Entity &entity, bool isRoot) {
    auto &data = entity.GetComponent<EntityDataComponent>();
    for (entt::entity entityId : data.children) {
        Entity child(entityId, this);
        DestroyEntity(child, false);
    }
    Entity parent(data.parent, this);
    if (isRoot && parent) {
        RemoveChildFromEntity(parent, entity);
    }
    destroy(entity);
}

void Scene::AddChildToEntity(Entity &parent, Entity &child) {
    if (parent == child) return;
    auto &parentData = parent.GetComponent<EntityDataComponent>();
    auto &childData = child.GetComponent<EntityDataComponent>();
    if (parentData.parent == child || childData.parent == parent) return;

    Transform *childTransform =
        &child.GetComponent<TransformComponent>().transform;
    Transform *parentTransform =
        &parent.GetComponent<TransformComponent>().transform;

    Entity oldParent(childData.parent, this);
    if (oldParent) {
        RemoveChildFromEntity(oldParent, child);
    }
    parentTransform->AddChild(childTransform);
    parentData.children.emplace(child);
    childData.parent = parent;
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
    auto ecsData = view<EntityDataComponent>();

    for (auto entityId : ecsData) {
        Entity entity(entityId, this);
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
        .component<IDComponent, EntityDataComponent, TagComponent,
                   TransformComponent, ModelComponent, LightComponent,
                   TextComponent>(archive);
    Refresh();
}

void Scene::Save(const std::string &filePath) {
    std::ofstream os(filePath, std::ios::binary);
    cereal::XMLOutputArchive archive(os);
    entt::snapshot{*this}
        .entities(archive)
        .component<IDComponent, EntityDataComponent, TagComponent,
                   TransformComponent, ModelComponent, LightComponent,
                   TextComponent>(archive);
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
