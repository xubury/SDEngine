cmake_minimum_required(VERSION 3.0.0)

project(Sandbox2D VERSION 0.1.0)

add_subdirectory(Engine)

add_executable(Sandbox2D Apps/src/Sandbox2DApp.cpp Apps/src/Sandbox2DLayer.cpp)

target_include_directories(Sandbox2D PUBLIC Apps/include)

set_property(TARGET Sandbox2D PROPERTY CXX_STANDARD 17)

target_compile_options(Sandbox2D PUBLIC  -Wall -Wextra -Wundef -pedantic)

target_link_libraries(Sandbox2D PUBLIC sd-renderer sd-layers)

