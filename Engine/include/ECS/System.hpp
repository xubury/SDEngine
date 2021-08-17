#ifndef SD_SYSTEM_HPP
#define SD_SYSTEM_HPP

#include <set>
#include "Common/Base.hpp"

namespace sd {

class System {
   public:
    System() = default;
    virtual ~System() = default;

    virtual void onTick(float dt) = 0;

    virtual void onRender() = 0;
};

class SystemManager {
   public:
    SystemManager() = default;

    template <typename SYSTEM, typename... ARGS>
    Ref<SYSTEM> addSystem(ARGS... args);

    void removeSystem(const Ref<System> &system);

    void tick(float dt);

    void render();

   private:
    std::set<Ref<System>> m_systems;
};

template <typename SYSTEM, typename... ARGS>
Ref<SYSTEM> SystemManager::addSystem(ARGS... args) {
    Ref<SYSTEM> system = createRef<SYSTEM>(std::forward<ARGS>(args)...);
    m_systems.emplace(system);
    return system;
}

}  // namespace sd

#endif /* SD_SYSTEM_HPP */
