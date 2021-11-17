#ifndef SD_VARIABLES_HPP
#define SD_VARIABLES_HPP

#include "Renderer/Renderer.hpp"
#include "Utility/EventDispatcher.hpp"
#include "Asset/Asset.hpp"

namespace SD {
#define APP_VARS             \
    Ref<Renderer> renderer;  \
    Ref<AssetManager> asset; \
    Ref<EventDispatcher> dispatcher;

#define SET_APP_VARS                       \
    void SetAppVars(const AppVars &vars) { \
        renderer = vars.renderer;          \
        asset = vars.asset;                \
        dispatcher = vars.dispatcher;      \
    }

#define MAKE_APP_VARS                 \
    AppVars MakeAppVars() {           \
        AppVars vars;                 \
        vars.renderer = renderer;     \
        vars.asset = asset;           \
        vars.dispatcher = dispatcher; \
        return vars;                  \
    }

struct AppVars {
    APP_VARS
};

}  // namespace SD

#endif  // !SD_VARIABLES_HPP
