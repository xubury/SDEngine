#ifndef SD_SYSTEM_HPP
#define SD_SYSTEM_HPP

#include "Utility/Base.hpp"
#include "Core/Export.hpp"
#include "Core/EventDispatcher.hpp"
#include "Core/Event.hpp"

namespace SD {

class SD_CORE_API System {
   public:
    System(const std::string &name) : m_name(name)
    {
        SD_CORE_TRACE("Initializing system: {}", name);
    }

    virtual ~System() { SD_CORE_TRACE("Deleting system: {}", m_name); };

    virtual void OnInit()
    {
        m_scene_handler = m_dispatcher->Register<NewSceneEvent>(
            [&](const NewSceneEvent &event) { m_scene = event.scene; });
    };

    virtual void OnDestroy() { m_dispatcher->RemoveHandler(m_scene_handler); }

    virtual void OnPush(){};

    virtual void OnPop(){};

    virtual void OnRender(){};

    virtual void OnImGui() {}

    virtual void OnTick(float) {}

    const std::string &GetName() const { return m_name; }

    EventDispatcher &GetEventDispatcher() { return *m_dispatcher; }

    Scene &GetScene() { return *m_scene; }

   private:
    friend class Layer;

    Ref<EventDispatcher> m_dispatcher;

    std::string m_name;

    HandlerRegistration m_scene_handler;

    Scene *m_scene;
};

}  // namespace SD

#endif /* SD_SYSTEM_HPP */
