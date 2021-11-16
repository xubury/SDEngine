#ifndef SD_LAYER_HPP
#define SD_LAYER_HPP

#include "Core/Vars.hpp"
#include "Core/Event.hpp"
#include "Core/EventStack.hpp"
#include "Core/System.hpp"
#include "Utility/Base.hpp"
#include "Utility/EventDispatcher.hpp"

#include <string>
#include <list>

namespace SD {

class SD_API Layer {
   public:
   public:
    Layer(const std::string &name) : m_name(name), m_blockEvent(false) {}

    virtual ~Layer() = default;

    Layer(const Layer &) = delete;

    Layer &operator=(const Layer &) = delete;

    virtual void onPush() {}

    virtual void onPop() {}

    virtual void onTick(float) {}

    virtual void onRender() {}

    virtual void onImGui() {}

    virtual void onEventProcess(const Event &) {}

    virtual void onEventsProcess() {}

    void setBlockEvent(bool block) { m_blockEvent = block; }
    bool isBlockEvent() const { return m_blockEvent; }

    template <typename SYSTEM, typename... ARGS>
    void pushSystem(ARGS &&...args) {
        SYSTEM *system = new SYSTEM(std::forward<ARGS>(args)...);
        pushSystem(system);
    }

    void pushSystem(System *system) {
        system->setAppVars(makeAppVars());
        system->onPush();
        m_systems.push(system);
    }

    template <typename SYSTEM>
    SYSTEM *getSystem(const std::string &name) {
        for (auto iter = m_systems.begin(); iter != m_systems.end(); ++iter) {
            if ((*iter)->getName() == name) {
                return dynamic_cast<SYSTEM *>(*iter);
            }
        }
        return nullptr;
    }

    void popSystem(System *system) {
        system->onPop();
        m_systems.pop(system);
    }

    void destroySystem(System *system) {
        popSystem(system);
        delete system;
    }

    const EventStack<System *> &getSystems() const { return m_systems; }
    EventStack<System *> &getSystems() { return m_systems; }

    const std::string &getName() const { return m_name; }

   protected:
    APP_VARS
    MAKE_APP_VARS;

   private:
    friend class Application;

    SET_APP_VARS;
    std::string m_name;
    bool m_blockEvent;
    EventStack<System *> m_systems;
};

}  // namespace SD

#endif /* SD_LAYER_HPP */
