#ifndef SD_VARIABLES_HPP
#define SD_VARIABLES_HPP

#include "Renderer/Renderer.hpp"
#include "Core/EventDispatcher.hpp"
#include "Asset/Asset.hpp"

namespace SD {

std::filesystem::path GetAppDirectory();

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

class Application;

Application &GetApp();

#define IMPLEMENT_APP(x)                                       \
    static ::SD::Application *s_instance;                      \
    ::SD::Application & ::SD::GetApp() { return *s_instance; } \
    ::SD::Application * ::SD::CreateApp() {                    \
        s_instance = new x;                                    \
        return s_instance;                                     \
    }

}  // namespace SD

#endif  // !SD_VARIABLES_HPP
