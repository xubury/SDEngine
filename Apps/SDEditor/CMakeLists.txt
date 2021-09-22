cmake_minimum_required(VERSION 3.0.0)

project(SDEditor VERSION 0.1.0)

add_executable(SDEditor
    src/SDEditor.cpp
    src/EditorLayer.cpp
    src/ScenePanel.cpp
    src/CameraController.cpp)

target_include_directories(SDEditor PUBLIC include)

set_property(TARGET SDEditor PROPERTY CXX_STANDARD 17)

target_compile_options(SDEditor PUBLIC  -Wall -Wextra -Wundef -pedantic)

target_link_libraries(SDEditor PUBLIC sd-core sd-utils)
