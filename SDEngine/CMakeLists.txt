if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_BUILD)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(SD_ENGINE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(src/Core)
add_subdirectory(src/Utility)
add_subdirectory(src/ECS)
add_subdirectory(src/ImGui)
add_subdirectory(src/Graphics)
add_subdirectory(src/Renderer)
add_subdirectory(src/Asset)
add_subdirectory(src/Loader)
add_subdirectory(src/System)
add_subdirectory(src/Animation)

add_subdirectory(libs/entt)
add_subdirectory(libs/sol2)
