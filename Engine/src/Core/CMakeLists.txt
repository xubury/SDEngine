find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

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
    ${Include_Root}/System.hpp
    ${Include_Root}/Keyboard.hpp
    ${Include_Root}/Mouse.hpp
    ${Include_Root}/Layer.hpp
    ${Include_Root}/ImGuiLayer.hpp
    ${Include_Root}/Event.hpp
    ${Include_Root}/Input.hpp)

set(Core_Src
    ${OpenGL_Src}
    ${Src_Root}/Application.cpp
    ${Src_Root}/Window.cpp
    ${Src_Root}/System.cpp
    ${Src_Root}/Keyboard.cpp
    ${Src_Root}/ImGuiLayer.cpp
    ${Src_Root}/Vars.cpp
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
    sd-renderer
    sd-utility
    sd-imgui
    PRIVATE
    SDL2::SDL2
    ${SDL2_IMAGE_LIBRARIES})

target_include_directories(sd-core
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include
    PRIVATE
    ${SDL2_IMAGE_INCLUDE_DIRS})

install(TARGETS sd-core LIBRARY DESTINATION lib)

install(FILES ${Core_Include} DESTINATION include/Core)
