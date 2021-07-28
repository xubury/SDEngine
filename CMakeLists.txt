cmake_minimum_required(VERSION 3.0.0)

# Simple and Direct Engine
project(SDEngine VERSION 0.1.0)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_BUILD)
    add_link_options(-mconsole)
else()
    add_link_options(-mwindows)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

add_subdirectory(src/Core)

add_subdirectory(src/Graphics)

add_executable(Simple2DApp Apps/Simple2D/src/Simple2DApp.cpp)
target_include_directories(Simple2DApp PUBLIC Apps/Simple2D/include)

set_property(TARGET Simple2DApp PROPERTY CXX_STANDARD 17)

target_compile_options(Simple2DApp PUBLIC  -Wall -Wextra -Wundef -pedantic)

target_link_libraries(Simple2DApp PUBLIC sd-graphics)

