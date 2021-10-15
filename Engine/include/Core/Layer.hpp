#ifndef SD_LAYER_HPP
#define SD_LAYER_HPP

#include <string>
#include <list>

#include "Core/SDL.hpp"
#include "Utility/Export.hpp"
#include "Utility/Base.hpp"
#include "Utility/EventDispatcher.hpp"

namespace sd {

class System;

class Scene;

class SD_API Layer {
   public:
    Layer(const std::string &name) : m_name(name), m_blockEvent(false) {}

    virtual ~Layer() = default;

    Layer(const Layer &) = delete;

    Layer &operator=(const Layer &) = delete;

    virtual void onAttach() {}

    virtual void onDetech() {}

    virtual void onTick(float) {}

    virtual void onRender() {}

    virtual void onImGui() {}

    virtual void onEventProcess(const SDL_Event &) {}

    virtual void onEventsProcess() {}

    void setBlockEvent(bool block) { m_blockEvent = block; }
    bool isBlockEvent() const { return m_blockEvent; }

    template <typename SYSTEM, typename... ARGS>
    SYSTEM *addSystem(ARGS &&...args);

    void removeSystem(const Ref<System> &system);
    std::list<Ref<System>> &getSystems() { return m_systems; }

    EventDispatcher &getDispatcher() { return m_dispatcher; }

   protected:
    std::string m_name;
    bool m_blockEvent;
    Scene *m_scene;

    std::list<Ref<System>> m_systems;
    EventDispatcher m_dispatcher;
};

template <typename SYSTEM, typename... ARGS>
SYSTEM *Layer::addSystem(ARGS &&...args) {
    Ref<SYSTEM> system = createRef<SYSTEM>(std::forward<ARGS>(args)...);
    system->m_layer = this;
    m_systems.emplace_back(system);
    system->onInit();
    return system.get();
}

}  // namespace sd

#endif /* SD_LAYER_HPP */
