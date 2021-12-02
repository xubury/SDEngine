find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(assimp REQUIRED)
find_package(Freetype REQUIRED)

set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/Asset)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/Asset)

set(Asset_Include
    ${Include_Root}/Asset.hpp
    ${Include_Root}/Font.hpp
    ${Include_Root}/Model.hpp
    ${Include_Root}/Image.hpp
    ${Include_Root}/ImageLoader.hpp
    ${Include_Root}/FontLoader.hpp
    ${Include_Root}/ShaderLoader.hpp
    ${Include_Root}/ModelLoader.hpp)

set(Asset_Src
    ${Src_Root}/Asset.cpp
    ${Src_Root}/Font.cpp
    ${Src_Root}/Model.cpp
    ${Src_Root}/Image.cpp
    ${Src_Root}/FontLoader.cpp
    ${Src_Root}/ImageLoader.cpp
    ${Src_Root}/ShaderLoader.cpp
    ${Src_Root}/ModelLoader.cpp)

add_library(sd-asset ${Asset_Src})

target_compile_options(sd-asset PUBLIC -Wall -Wextra -Wundef -pedantic -std=c++17)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-asset 
        INTERFACE
        SD_ASSET_IMPORT

        PRIVATE
        SD_ASSET_EXPORT)
endif()

target_include_directories(sd-asset
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include
    PRIVATE
    ${SDL2_IMAGE_INCLUDE_DIRS})

target_link_libraries(sd-asset
    PUBLIC
    sd-utility
    PRIVATE
    sd-graphics
    SDL2::SDL2
    ${SDL2_IMAGE_LIBRARIES}
    assimp::assimp
    Freetype::Freetype)

install(TARGETS sd-asset LIBRARY DESTINATION lib)

install(FILES ${Asset_Include} DESTINATION include/Asset)
