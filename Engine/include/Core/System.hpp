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

    virtual void onPush(){};

    virtual void onPop(){};

    virtual void onRender(){};

    virtual void onTick(float) {}

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
