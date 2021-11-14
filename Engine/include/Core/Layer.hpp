#ifndef SD_LAYER_HPP
#define SD_LAYER_HPP

#include "Core/Vars.hpp"
#include "Core/System.hpp"
#include "Utility/Base.hpp"
#include "Utility/EventDispatcher.hpp"

#include <string>
#include <list>

namespace SD {

class SD_API Layer {
   public:
    using SystemContainer = std::vector<System *>;

   public:
    Layer(const std::string &name) : m_name(name), m_blockEvent(false) {}

    virtual ~Layer() = default;

    Layer(const Layer &) = delete;

    Layer &operator=(const Layer &) = delete;

    virtual void onPush() {}

    virtual void onPop() {}

    void onDestroy() {
        while (m_systems.size() > 0) {
            destroySystem(m_systems.front());
        }
    }

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
        system->onInit();
        m_systems.push_back(system);
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
        auto itr = m_systems.begin();
        for (; itr != m_systems.end(); itr++) {
            if (*itr == system) {
                break;
            }
        }

        if (itr != m_systems.end()) {
            m_systems.erase(itr);
        } else {
            SD_CORE_ERROR("Layer::popSystem Failed! No system found!");
        }
        system->onPop();
    }

    void destroySystem(System *system) {
        popSystem(system);
        system->onDestroy();
        delete system;
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
