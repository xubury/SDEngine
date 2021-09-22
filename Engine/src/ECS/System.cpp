#include "ECS/System.hpp"
#include "Utils/Log.hpp"

namespace sd {

void SystemManager::removeSystem(const Ref<System> &system) {
    if (m_systems.find(system) != m_systems.end()) {
        m_systems.erase(system);
    } else {
        SD_CORE_WARN("System does not exist!");
    }
}

void SystemManager::tick(float dt) {
    for (const auto &system : m_systems) {
        system->onTick(dt);
    }
}

void SystemManager::render() {
    for (const auto &system : m_systems) {
        system->onRender();
    }
}

void SystemManager::setScene(Scene *scene) {
    for (auto &system : m_systems) {
        system->m_scene = scene;
        system->onSceneChange();
    }
}

}  // namespace sd
