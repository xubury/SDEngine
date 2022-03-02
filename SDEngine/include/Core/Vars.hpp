#ifndef SD_VARIABLES_HPP
#define SD_VARIABLES_HPP

#include "Core/Export.hpp"
#include "Core/Window.hpp"
#include "Renderer/Renderer.hpp"
#include "Utility/Ini.hpp"
#include "Utility/EventDispatcher.hpp"
#include "ECS/Scene.hpp"

namespace SD {

std::filesystem::path SD_CORE_API GetAppDirectory();

#define APP_VARS             \
    Ref<Ini> setting;        \
    Ref<Renderer> renderer;  \
    Ref<Scene> scene;        \

#define SET_APP_VARS                       \
    void SetAppVars(const AppVars &vars) { \
        setting = vars.setting;            \
        renderer = vars.renderer;          \
        scene = vars.scene;                \
    }

#define MAKE_APP_VARS                 \
    AppVars MakeAppVars() {           \
        AppVars vars;                 \
        vars.setting = setting;       \
        vars.renderer = renderer;     \
        vars.scene = scene;           \
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
        SD::GetApp().OnInit();           \
        SD::GetApp().Run();              \
        SD::GetApp().OnDestroy();        \
        return 0;                        \
    }

}  // namespace SD

#endif  // !SD_VARIABLES_HPP
