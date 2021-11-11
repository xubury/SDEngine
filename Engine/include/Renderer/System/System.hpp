#ifndef SD_SYSTEM_HPP
#define SD_SYSTEM_HPP

#include <set>
#include <list>
#include "Utility/EventDispatcher.hpp"
#include "Utility/Base.hpp"
#include "Renderer/System/Event.hpp"

namespace SD {

class System;

class SystemManager : public EventDispatcher {
   public:
    template <typename SYSTEM, typename... ARGS>
    Ref<SYSTEM> addSystem(ARGS &&...args);
    void removeSystem(const Ref<System> &system);

    std::list<Ref<System>> &getSystems() { return m_systems; }
    const std::list<Ref<System>> &getSystems() const { return m_systems; }

   private:
    std::list<Ref<System>> m_systems;
};

class System {
   public:
    System() = default;
    virtual ~System() = default;

    virtual void onInit() = 0;

    virtual void onDestroy() = 0;

    virtual void onRender() = 0;

    virtual void onTick(float) {}

    template <typename F, typename Event>
    void registerEvent(F *key, void (F::*listenerMethod)(const Event &)) {
        m_manager->addKeyedListener(key, listenerMethod);
    }

    template <typename Event, typename F>
    void unregisterEvent(F *object) {
        m_manager->removeKeyedListener<Event>(object);
    }

   private:
    friend class SystemManager;
    SystemManager *m_manager;
};

template <typename SYSTEM, typename... ARGS>
Ref<SYSTEM> SystemManager::addSystem(ARGS &&...args) {
    Ref<SYSTEM> system = createRef<SYSTEM>(std::forward<ARGS>(args)...);
    system->m_manager = this;
    m_systems.emplace_back(system);
    system->onInit();
    return system;
}

}  // namespace SD

#endif /* SD_SYSTEM_HPP */
