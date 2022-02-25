#ifndef SD_LAYER_HPP
#define SD_LAYER_HPP

#include "Utility/Base.hpp"
#include "Utility/EventStack.hpp"
#include "Core/Export.hpp"
#include "Core/Vars.hpp"
#include "Core/Event.hpp"
#include "Core/System.hpp"

#include <string>
#include <list>

namespace SD {

class SD_CORE_API Layer {
   public:
    Layer(const std::string &name) : m_name(name), m_is_block_event(false) {
        SD_CORE_TRACE("Initializing layer: {}", name);
    }

    virtual ~Layer() { SD_CORE_TRACE("Deleteing layer: {}", m_name); }

    Layer(const Layer &) = delete;

    Layer &operator=(const Layer &) = delete;
    virtual void OnInit() {
        m_scene_handler = dispatcher->Register<NewSceneEvent>(
            [&](const NewSceneEvent &event) {
                auto cam = scene->GetCamera();
                scene = event.scene;
                scene->SetCamera(cam);
            });
    }

    virtual void OnPush() {}

    virtual void OnPop() {}

    virtual void OnTick(float) {}

    virtual void OnRender() {}

    virtual void OnImGui() {}

    virtual void OnEventProcess(const Event &) {}

    void SetIsBlockEvent(bool is_block) { m_is_block_event = is_block; }
    bool IsBlockEvent() const { return m_is_block_event; }

    template <typename T, typename... ARGS>
    Ref<T> CreateSystem(ARGS &&...args) {
        Ref<T> system = CreateRef<T>(std::forward<ARGS>(args)...);
        system->SetAppVars(MakeAppVars());
        system->OnInit();
        return system;
    }

    void PushSystem(const Ref<System> &system) {
        system->OnPush();
        m_systems.Push(system);
    }

    void PopSystem(const Ref<System> &system) {
        system->OnPop();
        m_systems.Pop(system);
    }

    const EventStack<Ref<System>> &GetSystems() const { return m_systems; }
    EventStack<Ref<System>> &GetSystems() { return m_systems; }

    const std::string &GetName() const { return m_name; }

   protected:
    APP_VARS
    MAKE_APP_VARS;

   private:
    friend class Application;

    SET_APP_VARS;
    std::string m_name;
    bool m_is_block_event;
    EventStack<Ref<System>> m_systems;

    HandlerRegistration m_scene_handler;
};

}  // namespace SD

#endif /* SD_LAYER_HPP */
