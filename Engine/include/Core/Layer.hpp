#ifndef SD_LAYER_HPP
#define SD_LAYER_HPP

#include "Utility/Base.hpp"
#include "Utility/EventStack.hpp"
#include "Core/Vars.hpp"
#include "Core/Event.hpp"
#include "Core/System.hpp"

#include <string>
#include <list>

namespace SD {

class SD_API Layer {
   public:
   public:
    Layer(const std::string &name) : m_name(name), m_is_block_event(false) {}

    virtual ~Layer() = default;

    Layer(const Layer &) = delete;

    Layer &operator=(const Layer &) = delete;

    virtual void OnPush() {}

    virtual void OnPop() {}

    virtual void OnTick(float) {}

    virtual void OnRender() {}

    virtual void OnImGui() {}

    virtual void OnEventProcess(const Event &) {}

    virtual void OnEventsProcess() {}

    void SetIsBlockEvent(bool is_block) { m_is_block_event = is_block; }
    bool IsBlockEvent() const { return m_is_block_event; }

    template <typename SYSTEM, typename... ARGS>
    void PushSystem(ARGS &&...args) {
        SYSTEM *system = new SYSTEM(std::forward<ARGS>(args)...);
        PushSystem(system);
    }

    void PushSystem(System *system) {
        system->SetAppVars(MakeAppVars());
        system->OnPush();
        m_systems.Push(system);
    }

    template <typename SYSTEM>
    SYSTEM *GetSystem(const std::string &name) {
        for (auto iter = m_systems.begin(); iter != m_systems.end(); ++iter) {
            if ((*iter)->GetName() == name) {
                return dynamic_cast<SYSTEM *>(*iter);
            }
        }
        return nullptr;
    }

    void PopSystem(System *system) {
        system->OnPop();
        m_systems.Pop(system);
    }

    void DestroySystem(System *system) {
        PopSystem(system);
        delete system;
    }

    const EventStack<System *> &GetSystems() const { return m_systems; }
    EventStack<System *> &GetSystems() { return m_systems; }

    const std::string &GetName() const { return m_name; }

   protected:
    APP_VARS
    MAKE_APP_VARS;

   private:
    friend class Application;

    SET_APP_VARS;
    std::string m_name;
    bool m_is_block_event;
    EventStack<System *> m_systems;
};

}  // namespace SD

#endif /* SD_LAYER_HPP */
