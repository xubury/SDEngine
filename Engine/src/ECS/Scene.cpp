#include "ECS/Scene.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Component.hpp"
#include "Utility/Serialize.hpp"
#include "Asset/Asset.hpp"
#include <stack>

namespace SD {

Scene::Scene() {}

Entity Scene::createEntity(const std::string &name) {
    Entity entity(create(), this);
    entity.addComponent<IDComponent>();
    entity.addComponent<TagComponent>(name);
    entity.addComponent<TransformComponent>();
    entity.addComponent<EntityDataComponent>();
    return entity;
}

void Scene::destroyEntity(Entity &entity, bool isRoot) {
    auto &data = entity.getComponent<EntityDataComponent>();
    for (entt::entity entityId : data.children) {
        Entity child(entityId, this);
        destroyEntity(child, false);
    }
    Entity parent(data.parent, this);
    if (isRoot && parent) {
        removeChildFromEntity(parent, entity);
    }
    destroy(entity);
}

void Scene::addChildToEntity(Entity &parent, Entity &child) {
    if (parent == child) return;
    auto &parentData = parent.getComponent<EntityDataComponent>();
    auto &childData = child.getComponent<EntityDataComponent>();
    if (parentData.parent == child || childData.parent == parent) return;

    Transform *childTransform =
        &child.getComponent<TransformComponent>().transform;
    Transform *parentTransform =
        &parent.getComponent<TransformComponent>().transform;

    Entity oldParent(childData.parent, this);
    if (oldParent) {
        removeChildFromEntity(oldParent, child);
        childTransform->getParent()->removeChild(childTransform);
    }
    parentTransform->addChild(childTransform);
    parentData.children.emplace(child);
    childData.parent = parent;
}

void Scene::removeChildFromEntity(Entity &parent, Entity &child) {
    auto &children = parent.getComponent<EntityDataComponent>().children;
    if (children.find(child) != children.end()) {
        children.erase(child);
        Transform *childTransform =
            &child.getComponent<TransformComponent>().transform;
        parent.getComponent<TransformComponent>().transform.removeChild(
            childTransform);
    }
    child.getComponent<EntityDataComponent>().parent = Entity();
}

void Scene::refresh() {
    auto ecsData = view<EntityDataComponent>();

    for (auto entityId : ecsData) {
        Entity entity(entityId, this);
        refreshEntityChildTranforms(entity);
        refreshLight(entity);
    }
}

void Scene::load(const std::string &filePath) {
    clear();
    std::ifstream is(filePath, std::ios::binary);
    cereal::XMLInputArchive archive(is);
    entt::snapshot_loader{*this}
        .entities(archive)
        .component<IDComponent, EntityDataComponent, TagComponent,
                   TransformComponent, ModelComponent, LightComponent,
                   TextComponent>(archive);
    refresh();
}

void Scene::save(const std::string &filePath) {
    std::ofstream os(filePath, std::ios::binary);
    cereal::XMLOutputArchive archive(os);
    entt::snapshot{*this}
        .entities(archive)
        .component<IDComponent, EntityDataComponent, TagComponent,
                   TransformComponent, ModelComponent, LightComponent,
                   TextComponent>(archive);
}

void Scene::refreshLight(Entity &entity) {
    if (entity.hasComponent<LightComponent>()) {
        auto &lightComp = entity.getComponent<LightComponent>();
        if (lightComp.light.isCastShadow()) {
            lightComp.light.createShadowMap();
        }
    }
}

void Scene::refreshEntityChildTranforms(Entity &entity) {
    EntityDataComponent &data = entity.getComponent<EntityDataComponent>();
    Transform &transform = entity.getComponent<TransformComponent>().transform;
    transform.getChildren().clear();
    Entity parent(data.parent, this);
    if (parent) {
        parent.getComponent<TransformComponent>().transform.addChild(
            &transform);
    }

    for (entt::entity childId : data.children) {
        Entity child(childId, this);
        entity.getComponent<TransformComponent>().transform.addChild(
            &child.getComponent<TransformComponent>().transform);
        refreshEntityChildTranforms(child);
    }
}

}  // namespace SD
