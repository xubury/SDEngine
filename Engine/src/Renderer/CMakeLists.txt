find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(assimp REQUIRED)
find_package(Freetype REQUIRED)

set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/Renderer)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/Renderer)


set(Renderer_Include
    ${Include_Root}/Camera.hpp
    ${Include_Root}/Export.hpp
    ${Include_Root}/Light.hpp
    ${Include_Root}/Renderer.hpp
    ${Include_Root}/Skybox.hpp
    ${Include_Root}/Sprite.hpp
    ${Include_Root}/Terrain.hpp
    ${Include_Root}/Font.hpp
    ${Include_Root}/Model.hpp
    ${Include_Root}/Mateiral.hpp
    ${Include_Root}/Mesh.hpp
    ${Include_Root}/Bitmap.hpp
    ${Include_Root}/BitmapLoader.hpp
    ${Include_Root}/FontLoader.hpp
    ${Include_Root}/ModelLoader.hpp
    ${Include_Root}/SkyboxLoader.hpp)

set(Renderer_Src
    ${Src_Root}/Camera.cpp
    ${Src_Root}/Light.cpp
    ${Src_Root}/Renderer.cpp
    ${Src_Root}/Skybox.cpp
    ${Src_Root}/Sprite.cpp
    ${Src_Root}/Terrain.cpp
    ${Src_Root}/Font.cpp
    ${Src_Root}/Material.cpp
    ${Src_Root}/Mesh.cpp
    ${Src_Root}/Model.cpp
    ${Src_Root}/Bitmap.cpp
    ${Src_Root}/FontLoader.cpp
    ${Src_Root}/BitmapLoader.cpp
    ${Src_Root}/ModelLoader.cpp
    ${Src_Root}/SkyboxLoader.cpp)

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
    sd-asset
    PRIVATE
    SDL2::SDL2
    ${SDL2_IMAGE_LIBRARIES}
    assimp::assimp
    Freetype::Freetype)

target_include_directories(sd-renderer
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include
    PRIVATE
    ${SDL2_IMAGE_INCLUDE_DIRS})

install(TARGETS sd-renderer LIBRARY DESTINATION lib)

install(FILES ${Renderer_Include} DESTINATION include/Renderer)
