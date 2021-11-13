set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/Renderer)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/Renderer)

set(System_Include
    ${Include_Root}/System/Event.hpp
    ${Include_Root}/System/LightingSystem.hpp
    ${Include_Root}/System/ParticleSystem.hpp
    ${Include_Root}/System/PostProcessSystem.hpp
    ${Include_Root}/System/ProfileSystem.hpp
    ${Include_Root}/System/ShadowSystem.hpp
    ${Include_Root}/System/SkyboxSystem.hpp
    ${Include_Root}/System/SpriteRenderSystem.hpp
    ${Include_Root}/System/TerrainSystem.hpp)

set(System_Src
    ${Src_Root}/System/LightingSystem.cpp
    ${Src_Root}/System/ParticleSystem.cpp
    ${Src_Root}/System/PostProcessSystem.cpp
    ${Src_Root}/System/ProfileSystem.cpp
    ${Src_Root}/System/ShadowSystem.cpp
    ${Src_Root}/System/SkyboxSystem.cpp
    ${Src_Root}/System/SpriteRenderSystem.cpp
    ${Src_Root}/System/TerrainSystem.cpp)

set(Renderer_Include
    ${System_Include}
    ${Include_Root}/Camera.hpp
    ${Include_Root}/Font.hpp
    ${Include_Root}/Light.hpp
    ${Include_Root}/Mateiral.hpp
    ${Include_Root}/Mesh.hpp
    ${Include_Root}/Model.hpp
    ${Include_Root}/Renderer.hpp
    ${Include_Root}/RenderTarget.hpp
    ${Include_Root}/Terrain.hpp)

set(Renderer_Src
    ${System_Src}
    ${Src_Root}/Camera.cpp
    ${Src_Root}/Font.cpp
    ${Src_Root}/Light.cpp
    ${Src_Root}/Material.cpp
    ${Src_Root}/Mesh.cpp
    ${Src_Root}/Model.cpp
    ${Src_Root}/Renderer.cpp
    ${Src_Root}/RenderTarget.cpp
    ${Src_Root}/Terrain.cpp)

add_library(sd-renderer ${Renderer_Src})

set_property(TARGET sd-renderer PROPERTY CXX_STANDARD 17)

target_compile_options(sd-renderer PUBLIC -Wall -Wextra -Wundef -pedantic)

target_compile_definitions(sd-renderer PUBLIC SD_EXPORT)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-renderer PUBLIC SD_SHARED)
endif()

target_link_libraries(sd-renderer PUBLIC sd-graphics sd-ecs sd-core)

target_include_directories(sd-renderer
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include)

install(TARGETS sd-renderer LIBRARY DESTINATION lib)

install(FILES ${Renderer_Include} DESTINATION include/Renderer)
