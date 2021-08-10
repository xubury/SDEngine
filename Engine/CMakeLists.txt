cmake_minimum_required(VERSION 3.0.0)

# Simple and Direct Engine
project(SDEngine VERSION 0.1.0)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_BUILD)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

add_subdirectory(src/Core)
add_subdirectory(src/Common)
add_subdirectory(src/ECS)
add_subdirectory(src/ImGui)
add_subdirectory(src/Utils)
add_subdirectory(src/Graphics)
add_subdirectory(src/Renderer)

