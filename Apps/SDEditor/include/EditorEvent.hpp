#ifndef SD_EDITOR_EVENT_HPP
#define SD_EDITOR_EVENT_HPP

#include "ECS/Scene.hpp"

namespace SD {

struct EntitySelectEvent {
    EntityId entity_id{entt::null};
    Scene *scene{nullptr};
};

}  // namespace SD

#endif
