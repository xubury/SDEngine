cmake_minimum_required(VERSION 3.0.0)

project(S2D VERSION 0.1.0)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_BUILD)
endif()

find_package(SDL2)

set(Window_Src
    src/Window/Window.cpp)

add_executable(S2DEngine src/main.cpp src/Application.cpp ${Window_Src} )

set_property(TARGET S2DEngine PROPERTY CXX_STANDARD 17)

target_compile_options(S2DEngine PUBLIC -Wall -Wextra -Wundef -pedantic)

target_link_libraries(S2DEngine ${SDL2_LIBRARIES})

include_directories(include ${SDL2_INCLUDE_DIRS})
