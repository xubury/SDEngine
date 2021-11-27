if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_BUILD)
endif()

INCLUDE(FindPkgConfig)

set(SD_ENGINE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/Engine)

add_subdirectory(src/Core)
add_subdirectory(src/Utility)
add_subdirectory(src/ECS)
add_subdirectory(src/ImGui)
add_subdirectory(src/Input)
add_subdirectory(src/Graphics)
add_subdirectory(src/Renderer)
add_subdirectory(src/Asset)

add_subdirectory(libs/entt)
