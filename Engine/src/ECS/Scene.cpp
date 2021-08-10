#include "ECS/Scene.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Component.hpp"

namespace sd {

Entity Scene::createEntity(const std::string &name) {
    Entity entity(m_registry.create(), this);
    entity.addComponent<TagComponent>(name);
    return entity;
}

void Scene::destroyEntity(const Entity &entity) { m_registry.destroy(entity); }

}  // namespace sd
