#include "ECS/Scene.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Component.hpp"
#include "Utility/Serialize.hpp"
#include "Asset/Asset.hpp"

#include <stack>

namespace SD {

Scene::Scene() : m_selected_entity(Entity::INVALID_ID) {
    on_construct<TransformComponent>()
        .connect<&Scene::OnTransformComponentAdded>(this);
    on_construct<LightComponent>().connect<&Scene::OnLightComponentAdded>(this);
}

Entity Scene::CreateEntity(const std::string &name) {
    Entity entity(create(), this);
    entity.AddComponent<IdComponent>();
    entity.AddComponent<TagComponent>(name);
    entity.AddComponent<TransformComponent>();
    return entity;
}

void Scene::Save(const std::string &filePath) {
    std::ofstream os(filePath, std::ios::binary);
    cereal::PortableBinaryOutputArchive archive(os);
    entt::snapshot loader{*this};
    loader.entities(archive);
    for (auto &func : m_serialize_functions) {
        func.second.first(loader, archive);
    }
}

void Scene::Load(const std::string &filePath) {
    clear();
    std::ifstream is(filePath, std::ios::binary);
    cereal::PortableBinaryInputArchive archive(is);
    entt::snapshot_loader loader{*this};
    loader.entities(archive);
    for (auto &func : m_serialize_functions) {
        func.second.second(loader, archive);
    }
}

void Scene::OnTransformComponentAdded(entt::registry &reg, entt::entity ent) {
    auto &data = reg.get<TransformComponent>(ent);
    data.ecs = this;
}

void Scene::OnLightComponentAdded(entt::registry &reg, entt::entity ent) {
    auto &lightComp = reg.get<LightComponent>(ent);
    if (lightComp.light.IsCastShadow()) {
        lightComp.light.CreateShadowMap();
    }
}

void Scene::SetCamera(Camera *camera) { m_camera = camera; }

Camera *Scene::GetCamera() { return m_camera; }

Entity Scene::GetSelectedEntity() { return {m_selected_entity, this}; }

void Scene::ResetSelectedEntity() { m_selected_entity = Entity::INVALID_ID; }

}  // namespace SD
