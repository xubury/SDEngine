#include "ECS/Scene.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Component.hpp"
#include "Utility/Serialize.hpp"
#include "Asset/Asset.hpp"

#include <stack>

namespace SD {

Scene::Scene() : m_selected_entity(Entity::INVALID_ID) {}

Entity Scene::CreateEntity(const std::string &name) {
    Entity entity(create(), this);
    entity.AddComponent<IdComponent>();
    entity.AddComponent<TagComponent>(name);
    entity.AddComponent<TransformComponent>();
    return entity;
}

void Scene::Refresh() {
    auto ecs_data = view<TransformComponent>();

    for (auto entity_id : ecs_data) {
        Entity entity(entity_id, this);
        RefreshLight(entity);
    }
}

void Scene::Load(const std::string &filePath) {
    clear();
    std::ifstream is(filePath, std::ios::binary);
    cereal::XMLInputArchive archive(is);
    entt::snapshot_loader{*this}
        .entities(archive)
        .component<IdComponent, TagComponent, TransformComponent,
                   ModelComponent, LightComponent, TextComponent,
                   CameraComponent, TileLayoutComponent>(archive);
    Refresh();
}

void Scene::Save(const std::string &filePath) {
    std::ofstream os(filePath, std::ios::binary);
    cereal::XMLOutputArchive archive(os);
    entt::snapshot{*this}
        .entities(archive)
        .component<IdComponent, TagComponent, TransformComponent,
                   ModelComponent, LightComponent, TextComponent,
                   CameraComponent, TileLayoutComponent>(archive);
}

void Scene::RefreshLight(Entity &entity) {
    if (entity.HasComponent<LightComponent>()) {
        auto &lightComp = entity.GetComponent<LightComponent>();
        if (lightComp.light.IsCastShadow()) {
            lightComp.light.CreateShadowMap();
        }
    }
}

void Scene::SetCamera(Camera *camera) { m_camera = camera; }

Camera *Scene::GetCamera() { return m_camera; }

Entity Scene::GetSelectedEntity() { return {m_selected_entity, this}; }

void Scene::ResetSelectedEntity() { m_selected_entity = Entity::INVALID_ID; }

}  // namespace SD
