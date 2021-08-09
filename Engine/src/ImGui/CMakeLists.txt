find_package(imgui REQUIRED)

set(Include_Root ${PROJECT_SOURCE_DIR}/include/ImGui)
set(Src_Root ${PROJECT_SOURCE_DIR}/src/ImGui)

set(ImGui_Include ${Include_Root}/ImGuiLayer.hpp)

set(ImGui_Src
    ${Src_Root}/ImGuiLayer.cpp
    ${Src_Root}/imgui_impl_sdl.cpp
    ${Src_Root}/imgui_impl_opengl3.cpp)

add_library(sd-imgui ${ImGui_Src})

set_property(TARGET sd-imgui PROPERTY CXX_STANDARD 17)

target_compile_options(sd-imgui PUBLIC -Wall -Wextra -Wundef -pedantic)

target_compile_definitions(sd-imgui PUBLIC SD_EXPORT)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-imgui PUBLIC SD_SHARED)
endif()

target_link_libraries(sd-imgui PUBLIC sd-core imgui::imgui)

target_include_directories(sd-imgui
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include)


install(TARGETS sd-imgui LIBRARY DESTINATION lib)

install(FILES ${Graphics_Include} DESTINATION include/ImGui)
