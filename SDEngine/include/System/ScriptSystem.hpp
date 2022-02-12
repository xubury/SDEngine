#ifndef SD_SCRIPT_SYSTEM_HPP
#define SD_SCRIPT_SYSTEM_HPP

#include "System/Export.hpp"
#include "Core/System.hpp"

namespace SD {

class SD_SYSTEM_API ScriptSystem : public System {
   public:
    ScriptSystem();
    void OnInit();

   private:
};

}  // namespace SD

#endif
