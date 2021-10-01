set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/ECS)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/ECS)

set(ECS_Include
    ${Include_Root}/Component.hpp
    ${Include_Root}/Entity.hpp
    ${Include_Root}/ParticleSystem.hpp
    ${Include_Root}/RenderSystem.hpp
    ${Include_Root}/Scene.hpp
    ${Include_Root}/System.hpp
    ${Include_Root}/TerrainSystem.hpp)

set(ECS_Src
    ${Src_Root}/Component.cpp
    ${Src_Root}/Entity.cpp
    ${Src_Root}/ParticleSystem.cpp
    ${Src_Root}/RenderSystem.cpp
    ${Src_Root}/Scene.cpp
    ${Src_Root}/TerrainSystem.cpp)

add_library(sd-ecs ${ECS_Src})

set_property(TARGET sd-ecs PROPERTY CXX_STANDARD 17)

target_compile_options(sd-ecs PUBLIC -Wall -Wextra -Wundef -pedantic)

target_compile_definitions(sd-ecs PUBLIC SD_EXPORT)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-ecs PUBLIC SD_SHARED)
endif()

target_include_directories(sd-ecs
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include
    PRIVATE
    ${SD_ENGINE_SOURCE_DIR}/libs/entt)

target_link_libraries(sd-ecs PUBLIC sd-core sd-utils EnTT::EnTT)

install(TARGETS sd-ecs LIBRARY DESTINATION lib)

install(FILES ${ECS_Include} DESTINATION include/ECS)
