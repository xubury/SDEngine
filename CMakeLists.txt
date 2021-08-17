cmake_minimum_required(VERSION 3.0.0)

project(SDEditor VERSION 0.1.0)

add_subdirectory(Engine)

add_executable(SDEditor 
    Apps/src/SDEditor.cpp 
    Apps/src/Sandbox2DLayer.cpp
    Apps/src/EditorLayer.cpp
    Apps/src/FileDialog.cpp
    Apps/src/ScenePanel.cpp)

target_include_directories(SDEditor PUBLIC Apps/include)

set_property(TARGET SDEditor PROPERTY CXX_STANDARD 17)

target_compile_options(SDEditor PUBLIC  -Wall -Wextra -Wundef -pedantic)

target_link_libraries(SDEditor PUBLIC sd-core)

