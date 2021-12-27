find_package(SDL2 REQUIRED)

set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/ImGui)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/ImGui)

set(ImGui_Include
    ${Include_Root}/FileDialog.hpp
    ${Include_Root}/Export.hpp
    ${Include_Root}/ImGuiWidget.hpp)

set(ImGui_Src
    ${Src_Root}/FileDialog.cpp
    ${Src_Root}/ImGuiBuild.cpp
    ${Src_Root}/ImGuiWidget.cpp)

add_library(sd-imgui ${ImGui_Src})

target_compile_options(sd-imgui PUBLIC -Wall -Wextra -Wundef -pedantic -std=c++17)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-imgui 
        INTERFACE 
        SD_IMGUI_IMPORT

        PRIVATE 
        SD_IMGUI_EXPORT)
endif()

target_link_libraries(sd-imgui
    PUBLIC
    sd-utility
    sd-graphics
    sd-tilemap
    PRIVATE
    SDL2::SDL2)

target_include_directories(sd-imgui
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/libs/imgui
    ${SD_ENGINE_SOURCE_DIR}/libs/ImGuizmo
    ${SD_ENGINE_SOURCE_DIR}/libs/imgui/backends
    ${SD_ENGINE_SOURCE_DIR}/include)


install(TARGETS sd-imgui LIBRARY DESTINATION lib)

install(FILES ${ImGui_Include} DESTINATION include/ImGui)
