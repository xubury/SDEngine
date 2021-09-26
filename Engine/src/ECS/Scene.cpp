#include "ECS/Scene.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Component.hpp"
#include <stack>

namespace sd {

Entity Scene::createEntity(const std::string &name) {
    Entity entity(create(), this);
    entity.addComponent<TagComponent>(name);
    entity.addComponent<TransformComponent>();
    entity.addComponent<EntityDataComponent>();
    return entity;
}

void Scene::destroyEntity(Entity &entity, bool isRoot) {
    auto &data = entity.getComponent<EntityDataComponent>();
    for (entt::entity entityId : data.m_children) {
        Entity child(entityId, this);
        destroyEntity(child, false);
    }
    Entity parent(data.m_parent, this);
    if (isRoot && parent) {
        removeChildFromEntity(parent, entity);
    }
    destroy(entity);
}

void Scene::addChildToEntity(Entity &parent, Entity &child) {
    if (parent == child) return;
    auto &parentData = parent.getComponent<EntityDataComponent>();
    auto &childData = child.getComponent<EntityDataComponent>();
    if (parentData.m_parent == child || childData.m_parent == parent) return;

    Transform *childTransform =
        &child.getComponent<TransformComponent>().transform;
    Transform *parentTransform =
        &parent.getComponent<TransformComponent>().transform;

    Entity oldParent(childData.m_parent, this);
    if (oldParent) {
        removeChildFromEntity(oldParent, child);
        childTransform->getParent()->removeChild(childTransform);
    }
    parentTransform->addChild(childTransform);
    parentData.m_children.emplace(child);
    childData.m_parent = parent;
}

void Scene::removeChildFromEntity(Entity &parent, Entity &child) {
    auto &children = parent.getComponent<EntityDataComponent>().m_children;
    if (children.find(child) != children.end()) {
        children.erase(child);
        Transform *childTransform =
            &child.getComponent<TransformComponent>().transform;
        parent.getComponent<TransformComponent>().transform.removeChild(
            childTransform);
    }
    child.getComponent<EntityDataComponent>().m_parent = Entity();
}

void Scene::refresh() {
    auto ecsData = view<EntityDataComponent>();

    for (auto entityId : ecsData) {
        Entity entity(entityId, this);
        refreshEntityModel(entity);
        refreshEntityChildTranforms(entity);
    }
}

void Scene::load(const std::string &filePath) {
    clear();
    std::ifstream os(filePath, std::ios::binary);
    cereal::BinaryInputArchive archive(os);
    entt::snapshot_loader{*this}
        .entities(archive)
        .component<EntityDataComponent, TagComponent, TransformComponent,
                   ModelComponent, LightComponent>(archive);
    refresh();
}

void Scene::save(const std::string &filePath) {
    std::ofstream os(filePath, std::ios::binary);
    cereal::BinaryOutputArchive archive(os);
    entt::snapshot{*this}
        .entities(archive)
        .component<EntityDataComponent, TagComponent, TransformComponent,
                   ModelComponent, LightComponent>(archive);
}

void Scene::refreshEntityModel(Entity &entity) {
    if (entity.hasComponent<ModelComponent>()) {
        auto &modelComp = entity.getComponent<ModelComponent>();
        modelComp.model = Asset::manager().load<Model>(modelComp.path);
    }
}

void Scene::refreshEntityChildTranforms(Entity &entity) {
    EntityDataComponent &data = entity.getComponent<EntityDataComponent>();
    entity.getComponent<TransformComponent>().transform.getChildren().clear();
    Entity parent(data.m_parent, this);
    Transform *parentTransform =
        parent ? &parent.getComponent<TransformComponent>().transform : nullptr;
    entity.getComponent<TransformComponent>().transform.setParent(
        parentTransform);

    for (entt::entity childId : data.m_children) {
        Entity child(childId, this);
        entity.getComponent<TransformComponent>().transform.addChild(
            &child.getComponent<TransformComponent>().transform);
        refreshEntityChildTranforms(child);
    }
}

}  // namespace sd
