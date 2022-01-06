find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(assimp REQUIRED)
find_package(Freetype REQUIRED)

set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/Loader)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/Loader)

set(Loader_Include
    ${Include_Root}/Export.hpp
    ${Include_Root}/SpriteLoader.hpp
    ${Include_Root}/BitmapLoader.hpp
    ${Include_Root}/FontLoader.hpp
    ${Include_Root}/ModelLoader.hpp
    ${Include_Root}/ShaderLoader.hpp
    ${Include_Root}/SkyboxLoader.hpp)

set(Loader_Src
    ${Src_Root}/SpriteLoader.cpp
    ${Src_Root}/BitmapLoader.cpp
    ${Src_Root}/FontLoader.cpp
    ${Src_Root}/ModelLoader.cpp
    ${Src_Root}/ShaderLoader.cpp
    ${Src_Root}/SkyboxLoader.cpp)

add_library(sd-loader ${Loader_Src})

target_compile_options(sd-loader PUBLIC -Wall -Wextra -Wundef -pedantic -std=c++17)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-loader
        INTERFACE
        SD_LOADER_IMPORT

        PRIVATE
        SD_LOADER_EXPORT)
endif()

target_include_directories(sd-loader
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include)

target_link_libraries(sd-loader 
    PUBLIC 
    sd-utility 
    sd-graphics
    sd-renderer

    PRIVATE
    SDL2::SDL2
    ${SDL2_IMAGE_LIBRARIES}
    assimp::assimp
    Freetype::Freetype)


install(TARGETS sd-loader LIBRARY DESTINATION lib)

install(FILES ${Loader_Include} DESTINATION include/Loader)
