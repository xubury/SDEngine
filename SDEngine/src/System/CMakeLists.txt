set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/System)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/System)

set(System_Include
    ${Include_Root}/Export.hpp
    ${Include_Root}/CameraSystem.hpp
    ${Include_Root}/LightingSystem.hpp
    ${Include_Root}/ParticleSystem.hpp
    ${Include_Root}/PostProcessSystem.hpp
    ${Include_Root}/ProfileSystem.hpp
    ${Include_Root}/SkyboxSystem.hpp
    ${Include_Root}/SpriteRenderSystem.hpp
    ${Include_Root}/TerrainSystem.hpp)

set(System_Src
    ${Src_Root}/CameraSystem.cpp
    ${Src_Root}/LightingSystem.cpp
    ${Src_Root}/ParticleSystem.cpp
    ${Src_Root}/PostProcessSystem.cpp
    ${Src_Root}/ProfileSystem.cpp
    ${Src_Root}/SkyboxSystem.cpp
    ${Src_Root}/SpriteRenderSystem.cpp
    ${Src_Root}/TerrainSystem.cpp)

add_library(sd-system ${System_Src})

target_compile_options(sd-system PUBLIC -Wall -Wextra -Wundef -pedantic -std=c++17)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-system 
        INTERFACE 
        SD_SYSTEM_IMPORT 

        PRIVATE 
        SD_SYSTEM_EXPORT)
endif()

target_link_libraries(sd-system 
    PUBLIC 
    sd-ecs 
    sd-utility
    sd-core
    sd-asset)

target_include_directories(sd-system
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include)

install(TARGETS sd-system LIBRARY DESTINATION lib)

install(FILES ${System_Include} DESTINATION include/System)
