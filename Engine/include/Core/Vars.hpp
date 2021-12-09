#ifndef SD_VARIABLES_HPP
#define SD_VARIABLES_HPP

#include "Core/Export.hpp"
#include "Core/Window.hpp"
#include "Asset/Asset.hpp"
#include "Utility/Ini.hpp"
#include "Renderer/Renderer.hpp"
#include "Utility/EventDispatcher.hpp"
#include "ECS/Scene.hpp"

namespace SD {

std::filesystem::path SD_CORE_API GetAppDirectory();

#define APP_VARS             \
    Ref<Ini> ini;            \
    Ref<Window> window;      \
    Ref<Renderer> renderer;  \
    Ref<AssetManager> asset; \
    Ref<Scene> scene;        \
    Ref<EventDispatcher> dispatcher;

#define SET_APP_VARS                       \
    void SetAppVars(const AppVars &vars) { \
        ini = vars.ini;                    \
        window = vars.window;              \
        renderer = vars.renderer;          \
        asset = vars.asset;                \
        scene = vars.scene;                \
        dispatcher = vars.dispatcher;      \
    }

#define MAKE_APP_VARS                 \
    AppVars MakeAppVars() {           \
        AppVars vars;                 \
        vars.ini = ini;               \
        vars.window = window;         \
        vars.renderer = renderer;     \
        vars.asset = asset;           \
        vars.scene = scene;           \
        vars.dispatcher = dispatcher; \
        return vars;                  \
    }

struct AppVars {
    APP_VARS
};

class Application;

Application &GetApp();

#define IMPLEMENT_APP(x)                 \
    ::SD::Application & ::SD::GetApp() { \
        static x s_instance;             \
        return s_instance;               \
    }                                    \
    int main(int, char **) {             \
        SD::GetApp().OnStart();          \
        SD::GetApp().Run();              \
        SD::GetApp().OnExit();           \
        return 0;                        \
    }

}  // namespace SD

#endif  // !SD_VARIABLES_HPP
