#include "Core/Layer.hpp"

namespace sd {

void Layer::removeSystem(const Ref<System> &system) {
    auto iter = std::find(m_systems.begin(), m_systems.end(), system);
    if (iter != m_systems.end()) {
        m_systems.erase(iter);
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
