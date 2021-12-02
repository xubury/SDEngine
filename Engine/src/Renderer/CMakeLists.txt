set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/Renderer)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/Renderer)


set(Renderer_Include
    ${Include_Root}/Camera.hpp
    ${Include_Root}/Light.hpp
    ${Include_Root}/Renderer.hpp
    ${Include_Root}/RenderTarget.hpp
    ${Include_Root}/Skybox.hpp
    ${Include_Root}/Terrain.hpp)

set(Renderer_Src
    ${Src_Root}/Camera.cpp
    ${Src_Root}/Light.cpp
    ${Src_Root}/Renderer.cpp
    ${Src_Root}/RenderTarget.cpp
    ${Src_Root}/Skybox.cpp
    ${Src_Root}/Terrain.cpp)

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
    sd-asset)

target_include_directories(sd-renderer
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include)

install(TARGETS sd-renderer LIBRARY DESTINATION lib)

install(FILES ${Renderer_Include} DESTINATION include/Renderer)
