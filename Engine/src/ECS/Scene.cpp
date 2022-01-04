#include "ECS/Scene.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Component.hpp"
#include "Utility/Serialize.hpp"
#include "Asset/Asset.hpp"

#include <stack>

namespace SD {

Scene::Scene() : m_selected_entity(entt::null) {}

Entity Scene::CreateEntity(const std::string &name) {
    Entity entity(create(), this);
    entity.AddComponent<IdComponent>();
    entity.AddComponent<TagComponent>(name);
    entity.AddComponent<TransformComponent>();
    return entity;
}

Entity Scene::CloneEntity(EntityId from) {
    EntityId to = create();
    for (auto &&curr : storage()) {
        if (auto &storage = curr.second; storage.contains(from)) {
            storage.emplace(to, storage.get(from));
        }
    }
    return {to, this};
}

void Scene::Serialize(cereal::PortableBinaryOutputArchive &archive) {
    entt::snapshot loader{*this};
    loader.entities(archive);
    for (auto &func : m_serialize_functions) {
        func.second.first(loader, archive);
    }
}

void Scene::Deserialize(cereal::PortableBinaryInputArchive &archive) {
    entt::snapshot_loader loader{*this};
    loader.entities(archive);
    for (auto &func : m_serialize_functions) {
        func.second.second(loader, archive);
    }
}

void Scene::SetCamera(Camera *camera) { m_camera = camera; }

Camera *Scene::GetCamera() { return m_camera; }

}  // namespace SD
