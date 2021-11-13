#ifndef SD_VARIABLES_HPP
#define SD_VARIABLES_HPP

#include "Renderer/Renderer.hpp"
#include "Utility/EventDispatcher.hpp"

namespace SD {
#define APP_VARS            \
    Ref<Renderer> renderer; \
    Ref<EventDispatcher> dispatcher;

#define SET_APP_VARS                       \
    void setAppVars(const AppVars &vars) { \
        renderer = vars.renderer;          \
        dispatcher = vars.dispatcher;      \
    }

#define MAKE_APP_VARS                 \
    AppVars makeAppVars() {           \
        AppVars vars;                 \
        vars.renderer = renderer;     \
        vars.dispatcher = dispatcher; \
        return vars;                  \
    }

struct AppVars {
    APP_VARS
};

}  // namespace SD

#endif  // !SD_VARIABLES_HPP
