#include "Core/Layer.hpp"

namespace sd {

void Layer::removeSystem(const Ref<System> &system) {
    if (m_systems.find(system) != m_systems.end()) {
        m_systems.erase(system);
    } else {
        SD_CORE_WARN("System does not exist!");
    }
}

void Layer::setScene(Scene *scene) {
    for (auto &system : m_systems) {
        system->m_scene = scene;
        system->onSceneChange();
    }
}

}  // namespace sd
