#include "ECS/System.hpp"
#include "Common/Log.hpp"

namespace sd {

void SystemManager::addSystem(Ref<System> system) {
    if (m_systems.find(system) == m_systems.end()) {
        m_systems.emplace(system);
    } else {
        SD_CORE_WARN("System already exists!");
    }
}

void SystemManager::removeSystem(Ref<System> system) {
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

}  // namespace sd
