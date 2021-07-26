cmake_minimum_required(VERSION 3.0.0)

# Simple and direct engine
project(SDEngine VERSION 0.1.0)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_BUILD)
    add_link_options(-mconsole)
else()
    add_link_options(-mwindows)
endif()

find_package(SDL2 REQUIRED)

add_subdirectory(src/Core)

set(Window_Src
    src/Window/Window.cpp)

add_executable(SDEngine src/main.cpp src/Application.cpp ${Window_Src})

set_property(TARGET SDEngine PROPERTY CXX_STANDARD 17)

target_compile_options(SDEngine PUBLIC  -Wall -Wextra -Wundef -pedantic)

if(MINGW)
target_link_libraries(SDEngine mingw32)
endif()

target_link_libraries(SDEngine ${SDL2_LIBRARIES} sd-core)

include_directories(include ${SDL2_INCLUDE_DIRS})
