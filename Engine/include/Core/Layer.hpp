#ifndef SD_LAYER_HPP
#define SD_LAYER_HPP

#include "Core/Vars.hpp"
#include "Core/System.hpp"
#include "Utility/Base.hpp"
#include "Utility/EventDispatcher.hpp"

#include <string>
#include <list>
#include <SDL.h>

namespace SD {

class Renderer;
class System;

class SD_API Layer {
   public:
    using SystemContainer = std::list<Ref<System>>;

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
    void pushSystem(ARGS &&...args) {
        Ref<SYSTEM> system = createRef<SYSTEM>(std::forward<ARGS>(args)...);
        system->setAppVars(makeAppVars());
        system->onInit();
        m_systems.push_back(system);
    }

    template <typename SYSTEM>
    SYSTEM *getSystem(const std::string &name) {
        for (auto iter = m_systems.begin(); iter != m_systems.end(); ++iter) {
            if ((*iter)->getName() == name) {
                return dynamic_cast<SYSTEM *>((*iter).get());
            }
        }
        return nullptr;
    }

    const SystemContainer &getSystems() const { return m_systems; }
    SystemContainer &getSystems() { return m_systems; }

   protected:
    APP_VARS

   private:
    friend class Application;

    SET_APP_VARS;
    MAKE_APP_VARS;
    std::string m_name;
    bool m_blockEvent;
    EventDispatcher m_dispatcher;
    SystemContainer m_systems;
};

}  // namespace SD

#endif /* SD_LAYER_HPP */
