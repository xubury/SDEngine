#ifndef SD_LAYER_HPP
#define SD_LAYER_HPP

#include "Utility/Base.hpp"
#include "Utility/EventStack.hpp"
#include "Utility/EventDispatcher.hpp"
#include "Core/Export.hpp"
#include "Core/Event/Event.hpp"
#include "Core/System.hpp"

#include <string>
#include <list>

namespace SD {

class SD_CORE_API Layer {
   public:
    Layer(const std::string &name) : m_name(name)
    {
        SD_CORE_TRACE("Initializing layer: {}", name);
    }

    virtual ~Layer() { SD_CORE_TRACE("Deleteing layer: {}", m_name); }

    Layer(const Layer &) = delete;

    Layer &operator=(const Layer &) = delete;

    virtual void OnInit()
    {
        m_scene_handler = m_dispatcher->Register<NewSceneEvent>(
            [&](const NewSceneEvent &event) { m_scene = event.scene; });
    }

    virtual void OnDestroy()
    {
        while (m_systems.Size()) {
            auto system = m_systems.Front();
            PopSystem(system);
            DestorySystem(system);
        }
        m_dispatcher->RemoveHandler(m_scene_handler);
    }

    virtual void OnPush() {}

    virtual void OnPop() {}

    virtual void OnTick(float) {}

    virtual void OnRender() {}

    virtual void OnImGui() {}

    template <typename T, typename... ARGS>
    T *CreateSystem(ARGS &&...args)
    {
        T *system = new T(std::forward<ARGS>(args)...);
        system->m_dispatcher = m_dispatcher;
        system->OnInit();
        return system;
    }

    void DestorySystem(System *system) { delete system; }

    void PushSystem(System *system)
    {
        system->OnPush();
        m_systems.Push(system);
    }

    void PopSystem(System *system)
    {
        system->OnPop();
        m_systems.Pop(system);
    }

    const EventStack<System *> &GetSystems() const { return m_systems; }
    EventStack<System *> &GetSystems() { return m_systems; }

    const std::string &GetName() const { return m_name; }

    EventDispatcher &GetEventDispatcher() { return *m_dispatcher; }

    Scene &GetScene() { return *m_scene; }

   private:
    friend class Application;

    std::string m_name;
    EventStack<System *> m_systems;
    Ref<EventDispatcher> m_dispatcher;

    HandlerRegistration m_scene_handler;

    Scene *m_scene;
};

}  // namespace SD

#endif /* SD_LAYER_HPP */
