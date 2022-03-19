find_package(SDL2 REQUIRED)
find_package(Lua REQUIRED)

set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/Core)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/Core)

set(OpenGL_Include
    ${Include_Root}/OpenGL/GLWindow.hpp)

set(OpenGL_Src
    ${Src_Root}/OpenGL/GLWindow.cpp)

set(Event_Include
    ${Include_Root}/Event/Event.hpp)

set(Event_Src
    ${Src_Root}/Event/Event.cpp)

set(Core_Include
    ${OpenGL_Include}
    ${Event_Include}
    ${Include_Root}/Application.hpp
    ${Include_Root}/Export.hpp
    ${Include_Root}/Window.hpp
    ${Include_Root}/System.hpp
    ${Include_Root}/Keyboard.hpp
    ${Include_Root}/Mouse.hpp
    ${Include_Root}/Layer.hpp
    ${Include_Root}/ImGuiLayer.hpp
    ${Include_Root}/Event.hpp
    ${Include_Root}/Input.hpp)

set(Core_Src
    ${OpenGL_Src}
    ${Event_Src}
    ${Src_Root}/Application.cpp
    ${Src_Root}/Window.cpp
    ${Src_Root}/System.cpp
    ${Src_Root}/Keyboard.cpp
    ${Src_Root}/ImGuiLayer.cpp
    ${Src_Root}/Input.cpp)

add_library(sd-core ${Core_Src})

target_compile_options(sd-core PUBLIC -Wall -Wextra -Wundef -pedantic -std=c++17)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-core
        INTERFACE
        SD_CORE_IMPORT

        PRIVATE
        SD_CORE_EXPORT)
endif()

target_link_libraries(sd-core
    PUBLIC
    sd-utility
    sd-imgui
    sd-ecs
    sd-asset

    ${LUA_LIBRARIES}
    sol2

    PRIVATE
    SDL2::SDL2)

target_include_directories(sd-core
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include)

install(TARGETS sd-core LIBRARY DESTINATION lib)

install(FILES ${Core_Include} DESTINATION include/Core)
