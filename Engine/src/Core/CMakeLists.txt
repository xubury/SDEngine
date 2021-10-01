find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)

set(Include_Root ${PROJECT_SOURCE_DIR}/include/Core)
set(Src_Root ${PROJECT_SOURCE_DIR}/src/Core)

set(OpenGL_Include
    ${Include_Root}/OpenGL/GLContext.hpp)

set(OpenGL_Src
    ${Src_Root}/OpenGL/GLContext.cpp)

set(Core_Include
    ${OpenGL_Include}
    ${Include_Root}/Action.hpp
    ${Include_Root}/ActionMap.hpp
    ${Include_Root}/ActionTarget.hpp
    ${Include_Root}/Application.hpp
    ${Include_Root}/Context.hpp
    ${Include_Root}/EntryPoint.hpp
    ${Include_Root}/Input.hpp
    ${Include_Root}/Layer.hpp
    ${Include_Root}/LayerStack.hpp
    ${Include_Root}/RenderEngine.hpp
    ${Include_Root}/Timing.hpp
    ${Include_Root}/Window.hpp)

set(Core_Src
    ${OpenGL_Src}
    ${Src_Root}/Action.cpp
    ${Src_Root}/Application.cpp
    ${Src_Root}/Context.cpp
    ${Src_Root}/EntryPoint.cpp
    ${Src_Root}/Input.cpp
    ${Src_Root}/Layer.cpp
    ${Src_Root}/LayerStack.cpp
    ${Src_Root}/RenderEngine.cpp
    ${Src_Root}/Timing.cpp
    ${Src_Root}/Window.cpp)

add_library(sd-core ${Core_Src})

set_property(TARGET sd-core PROPERTY CXX_STANDARD 17)

target_compile_options(sd-core PUBLIC -Wall -Wextra -Wundef -pedantic)

target_compile_definitions(sd-core PUBLIC SD_EXPORT)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-core PUBLIC SD_SHARED)
endif()

target_link_libraries(sd-core PUBLIC sd-utils sd-imgui sd-renderer sd-ecs SDL2::Main GLEW::GLEW)

target_include_directories(sd-core 
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include)

install(TARGETS sd-core LIBRARY DESTINATION lib)

install(FILES ${Core_Include} DESTINATION include/Core)
