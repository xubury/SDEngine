cmake_minimum_required(VERSION 3.0.0)

# Simple and Direct Engine
project(SDEngine VERSION 0.1.0)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_BUILD)
    add_link_options(-mconsole)
else()
    add_link_options(-mwindows)
endif()

add_subdirectory(src/Core)

add_executable(SDEngine src/main.cpp)

set_property(TARGET SDEngine PROPERTY CXX_STANDARD 17)

target_compile_options(SDEngine PUBLIC  -Wall -Wextra -Wundef -pedantic)

target_link_libraries(SDEngine sd-core)

include_directories(include)
