#include "Renderer/System/System.hpp"

namespace SD {

void SystemManager::removeSystem(const Ref<System> &system) {
    auto iter = std::find(m_systems.begin(), m_systems.end(), system);
    if (iter != m_systems.end()) {
        system->onDestroy();
        m_systems.erase(iter);
    } else {
        SD_CORE_WARN("System does not exist!");
    }
}

}  // namespace SD
