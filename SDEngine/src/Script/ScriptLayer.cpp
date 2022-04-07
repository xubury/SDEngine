#include "Script/ScriptLayer.hpp"
#include "sol/sol.hpp"

namespace SD {

ScriptLayer::ScriptLayer() : Layer("ScriptLayer")
{
    // lua test
    sol::state lua;
    int x = 0;
    lua.set_function("beep", [&x] { ++x; });
    lua.script("beep()");
    SD_CORE_INFO("lua test: x={}", x);
}

}  // namespace SD
