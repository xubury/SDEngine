
#ifndef SD_EDITOR_EVENT_HPP
#define SD_EDITOR_EVENT_HPP

namespace SD {

struct EntitySelectEvent {
    EntityId entity_id;
    Scene *scene;
};

}  // namespace SD

#endif
