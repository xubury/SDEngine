#include "ECS/Scene.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Component.hpp"
#include <stack>

namespace sd {

Entity Scene::createEntity(const std::string &name) {
    Entity entity(m_registry.create(), this);
    entity.addComponent<TagComponent>(name);
    entity.addComponent<TransformComponent>();
    entity.addComponent<EntityDataComponent>();
    return entity;
}

void Scene::destroyEntity(Entity entity, bool isRoot) {
    auto &data = entity.getComponent<EntityDataComponent>();
    for (entt::entity entityId : data.m_children) {
        destroyEntity({entityId, this}, false);
    }
    sd::Entity parent(data.m_parent, this);
    if (isRoot && parent) {
        removeChildFromEntity(parent, entity);
    }
    m_registry.destroy(entity);
}

void Scene::addChildToEntity(Entity parent, Entity child) {
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

void Scene::removeChildFromEntity(Entity parent, Entity child) {
    auto &children = parent.getComponent<EntityDataComponent>().m_children;
    if (children.find(child) != children.end()) {
        children.erase(child);
        Transform *childTransform =
            &child.getComponent<TransformComponent>().transform;
        parent.getComponent<TransformComponent>().transform.removeChild(
            childTransform);
    }
    child.getComponent<EntityDataComponent>().m_parent = sd::Entity();
}

void Scene::clear() { m_registry.clear(); }

void Scene::refresh() {
    auto view = m_registry.view<EntityDataComponent>();

    for (auto entityId : view) {
        Entity entity(entityId, this);
        if (entity.hasComponent<ModelComponent>()) {
            auto &modelComp = entity.getComponent<ModelComponent>();
            modelComp.model =
                Graphics::assetManager().load<Model>(modelComp.path);
        }
        addEntityChildTranforms(entity);
    }
}

const entt::registry &Scene::getRegistry() const { return m_registry; }

entt::registry &Scene::getRegistry() { return m_registry; }

void Scene::addEntityChildTranforms(Entity entity) {
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
        addEntityChildTranforms(child);
    }
}

}  // namespace sd
