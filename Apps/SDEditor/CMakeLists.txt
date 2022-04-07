if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_BUILD)
endif()

cmake_minimum_required(VERSION 3.1.0)

project(SDEditor VERSION 0.1.0)

add_executable(SDEditor
    src/SDEditor.cpp
    src/EditorLayer.cpp
    src/ScenePanel.cpp
    src/ContentBrowser.cpp
    src/EditorCamera.cpp
    src/AnimationEditor.cpp
    src/TileMapEditor.cpp
    src/TileBrush.cpp)

target_include_directories(SDEditor PUBLIC include)

target_compile_options(SDEditor PUBLIC  -Wall -Wextra -Wundef -pedantic -std=c++17)

target_link_libraries(SDEditor PUBLIC sd-core sd-animation)
