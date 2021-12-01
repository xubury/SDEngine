#ifndef SD_SYSTEM_HPP
#define SD_SYSTEM_HPP

#include "Core/Export.hpp"
#include "Utility/Base.hpp"
#include "Core/Vars.hpp"
#include "Core/Event.hpp"

namespace SD {

class SD_CORE_API System {
   public:
    System(const std::string &name) : m_name(name) {
        SD_CORE_TRACE("Initializing system: {}", name);
    }

    virtual ~System() { SD_CORE_TRACE("Deleting system: {}", m_name); };

    virtual void OnInit(){};

    virtual void OnPush(){};

    virtual void OnPop(){};

    virtual void OnRender(){};

    virtual void OnTick(float) {}

    const std::string &GetName() const { return m_name; }

   protected:
    APP_VARS
    MAKE_APP_VARS

   private:
    friend class Layer;

    std::string m_name;

    SET_APP_VARS
};

}  // namespace SD

#endif /* SD_SYSTEM_HPP */
