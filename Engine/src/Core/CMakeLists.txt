find_package(SDL2 REQUIRED)

set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/Core)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/Core)

set(OpenGL_Include
    ${Include_Root}/OpenGL/GLWindow.hpp)

set(OpenGL_Src
    ${Src_Root}/OpenGL/GLWindow.cpp)

set(Core_Include
    ${OpenGL_Include}
    ${Include_Root}/Application.hpp
    ${Include_Root}/Window.hpp
    ${Include_Root}/EntryPoint.hpp
    ${Include_Root}/Layer.hpp
    ${Include_Root}/LayerStack.hpp
    ${Include_Root}/SDL.hpp)

set(Core_Src
    ${OpenGL_Src}
    ${Src_Root}/Application.cpp
    ${Src_Root}/Window.cpp
    ${Src_Root}/EntryPoint.cpp
    ${Src_Root}/LayerStack.cpp)

add_library(sd-core ${Core_Src})

set_property(TARGET sd-core PROPERTY CXX_STANDARD 17)

target_compile_options(sd-core PUBLIC -Wall -Wextra -Wundef -pedantic)

target_compile_definitions(sd-core PUBLIC SD_EXPORT)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-core PUBLIC SD_SHARED)
endif()

target_link_libraries(sd-core PUBLIC sd-renderer sd-utils sd-imgui sd-input SDL2::Main)

target_include_directories(sd-core 
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include)

install(TARGETS sd-core LIBRARY DESTINATION lib)

install(FILES ${Core_Include} DESTINATION include/Core)
