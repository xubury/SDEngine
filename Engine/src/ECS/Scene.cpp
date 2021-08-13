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

void Scene::destroyEntity(Entity &entity, bool isRoot) {
    auto &data = entity.getComponent<EntityDataComponent>();
    for (sd::Entity child : data.m_children) {
        destroyEntity(child, false);
    }
    sd::Entity parent = data.m_parent;
    if (isRoot && parent) {
        removeChildFromEntity(parent, entity);
    }
    m_registry.destroy(entity);
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

    if (childData.m_parent) {
        removeChildFromEntity(childData.m_parent, child);
        childTransform->parent()->removeChild(childTransform);
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
    child.getComponent<EntityDataComponent>().m_parent = sd::Entity();
}

const entt::registry &Scene::getRegistry() const { return m_registry; }

entt::registry &Scene::getRegistry() { return m_registry; }

}  // namespace sd
