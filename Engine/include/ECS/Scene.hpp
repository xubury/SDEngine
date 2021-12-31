#ifndef SD_SCENE_HPP
#define SD_SCENE_HPP

#include "Utility/Base.hpp"
#include "Utility/Serialize.hpp"
#include "ECS/Export.hpp"
#include "ECS/Component.hpp"

#define ENTT_USE_ATOMIC
#include "entt/config/config.h"
#include "entt/entity/snapshot.hpp"
#include "entt/entity/registry.hpp"
#include "entt/entity/entity.hpp"

namespace SD {

class Entity;

typedef entt::delegate<void(entt::snapshot &,
                            cereal::PortableBinaryOutputArchive &)>
    ComponentSerializeFunction;
typedef entt::delegate<void(entt::snapshot_loader &,
                            cereal::PortableBinaryInputArchive &)>

    ComponentDeserializeFunction;

class SD_ECS_API Scene : public entt::registry {
   public:
    Scene();
    ~Scene() = default;

    Entity CreateEntity(const std::string &name);

    void Load(const std::string &filePath);

    void Save(const std::string &filePath);

    void SetCamera(Camera *camera);

    Camera *GetCamera();

    void SetSelectedEntity(entt::entity entity) { m_selected_entity = entity; }
    Entity GetSelectedEntity();
    void ResetSelectedEntity();

    // Registering a component enables serialization as well as duplication
    // functionality in & out editor.
    template <typename T>
    void RegisterComponent(bool only_clone = false) {
        auto id = entt::type_hash<T>::value();
        if (!only_clone) {
            m_serialize_functions[id]
                .first.template connect<&Scene::SerializeComponent<T>>(this);
            m_serialize_functions[id]
                .second.template connect<&Scene::DeserializeComponent<T>>(this);
        }
    }

    void OnTransformComponentAdded(entt::registry &reg, entt::entity ent);
    void OnLightComponentAdded(entt::registry &reg, entt::entity ent);

   private:
    template <typename T>
    void SerializeComponent(entt::snapshot &snapshot,
                            cereal::PortableBinaryOutputArchive &archive) {
        snapshot.component<T>(archive);
    }

    template <typename T>
    void DeserializeComponent(entt::snapshot_loader &loader,
                              cereal::PortableBinaryInputArchive &archive) {
        loader.component<T>(archive);
    }

    Camera *m_camera;
    entt::entity m_selected_entity;
    std::unordered_map<entt::id_type, std::pair<ComponentSerializeFunction,
                                                ComponentDeserializeFunction>>
        m_serialize_functions;
};

}  // namespace SD

#endif /* SD_SCENE_HPP */
