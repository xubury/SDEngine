#include "Core/System.hpp"

namespace SD {

// void SystemManager::pushSystem(const Ref<System> &system) {
//     m_systems.push_back(system);
//     system->m_manager = this;
//     system->onInit();
// }

// void SystemManager::popSystem(const Ref<System> &system) {
//     auto iter = std::find(m_systems.begin(), m_systems.end(), system);
//     if (iter != m_systems.end()) {
//         system->onDestroy();
//         m_systems.erase(iter);
//     } else {
//         SD_CORE_WARN("System does not exist!");
//     }
// }

}  // namespace SD
