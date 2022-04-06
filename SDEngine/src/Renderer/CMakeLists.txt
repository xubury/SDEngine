set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/Renderer)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/Renderer)


set(Renderer_Include
    ${Include_Root}/Export.hpp
    ${Include_Root}/Event.hpp
    ${Include_Root}/RenderSystem.hpp
    ${Include_Root}/DeferredRenderer.hpp
    ${Include_Root}/PostProcessRenderer.hpp
    ${Include_Root}/SkyboxRenderer.hpp
    ${Include_Root}/SpriteRenderer.hpp
    ${Include_Root}/Renderer2D.hpp
    ${Include_Root}/Renderer3D.hpp)

set(Renderer_Src
    ${Src_Root}/DeferredRenderer.cpp
    ${Src_Root}/PostProcessRenderer.cpp
    ${Src_Root}/SkyboxRenderer.cpp
    ${Src_Root}/SpriteRenderer.cpp
    ${Src_Root}/Renderer.cpp
    ${Src_Root}/Renderer2D.cpp
    ${Src_Root}/Renderer3D.cpp)

add_library(sd-renderer ${Renderer_Src})

target_compile_options(sd-renderer PUBLIC -Wall -Wextra -Wundef -pedantic -std=c++17)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-renderer 
        INTERFACE
        SD_RENDERER_IMPORT

        PRIVATE 
        SD_RENDERER_EXPORT)
endif()

target_link_libraries(sd-renderer
    PUBLIC
    sd-utility
    sd-graphics
    sd-imgui
    sd-ecs
    sd-asset
    sd-loader)

target_include_directories(sd-renderer
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include
    PRIVATE
    ${SDL2_IMAGE_INCLUDE_DIRS})

install(TARGETS sd-renderer LIBRARY DESTINATION lib)

install(FILES ${Renderer_Include} DESTINATION include/Renderer)
