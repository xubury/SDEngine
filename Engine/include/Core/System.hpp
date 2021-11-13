#ifndef SD_SYSTEM_HPP
#define SD_SYSTEM_HPP

#include "Core/Vars.hpp"
#include "Core/Event.hpp"
#include "Utility/EventDispatcher.hpp"
#include "Utility/Base.hpp"

namespace SD {

class Layer;

class System {
   public:
    System(const std::string &name) : m_name(name) {}

    virtual ~System() = default;

    virtual void onPush() {}

    virtual void onPop() {}

    virtual void onInit(){};

    virtual void onDestroy(){};

    virtual void onRender(){};

    virtual void onTick(float) {}

    template <typename F, typename Event>
    void registerEvent(F *key, void (F::*listenerMethod)(const Event &)) {
        dispatcher->addKeyedListener(key, listenerMethod);
    }

    template <typename Event, typename F>
    void unregisterEvent(F *object) {
        dispatcher->removeKeyedListener<Event>(object);
    }

    const std::string &getName() const { return m_name; }

   protected:
    APP_VARS

   private:
    friend class Layer;

    std::string m_name;

    SET_APP_VARS
    MAKE_APP_VARS
};

}  // namespace SD

#endif /* SD_SYSTEM_HPP */
