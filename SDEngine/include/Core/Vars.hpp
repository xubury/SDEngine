#ifndef SD_VARIABLES_HPP
#define SD_VARIABLES_HPP

#include "Core/Export.hpp"
#include "Utility/Ini.hpp"
#include "ECS/Scene.hpp"

namespace SD {

std::filesystem::path SD_CORE_API GetAppDirectory();

class Application;

Application &GetApp();

#define IMPLEMENT_APP(x)               \
    ::SD::Application & ::SD::GetApp() \
    {                                  \
        static x s_instance;           \
        return s_instance;             \
    }                                  \
    int main(int, char **)             \
    {                                  \
        SD::GetApp().OnInit();         \
        SD::GetApp().Run();            \
        SD::GetApp().OnDestroy();      \
        return 0;                      \
    }

}  // namespace SD

#endif  // !SD_VARIABLES_HPP
