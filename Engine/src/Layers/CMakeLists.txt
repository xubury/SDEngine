find_package(imgui REQUIRED)

set(Include_Root ${PROJECT_SOURCE_DIR}/include/Layers)
set(Src_Root ${PROJECT_SOURCE_DIR}/src/Layers)

set(Layers_Include ${Include_Root}/ImGuiLayer.hpp)

set(Layers_Src
    ${Src_Root}/ImGuiLayer.cpp
    ${Src_Root}/imgui_impl_sdl.cpp
    ${Src_Root}/imgui_impl_opengl3.cpp)

add_library(sd-layers ${Layers_Src})

set_property(TARGET sd-layers PROPERTY CXX_STANDARD 17)

target_compile_options(sd-layers PUBLIC -Wall -Wextra -Wundef -pedantic)

target_compile_definitions(sd-layers PUBLIC SD_EXPORT)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-layers PUBLIC SD_SHARED)
endif()

target_link_libraries(sd-layers PUBLIC sd-core imgui::imgui)

target_include_directories(sd-layers
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include)


install(TARGETS sd-layers LIBRARY DESTINATION lib)

install(FILES ${Graphics_Include} DESTINATION include/Layers)
