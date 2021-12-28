set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/TileMap)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/TileMap)

set(TileMap_Include
    ${Include_Root}/Export.hpp
    ${Include_Root}/TileLayout.hpp
    ${Include_Root}/Sprite.hpp
    ${Include_Root}/TileMap.hpp
    ${Include_Root}/TileMapLoader.hpp)

set(TileMap_Src
    ${Src_Root}/Sprite.cpp
    ${Src_Root}/TileMap.cpp
    ${Src_Root}/TileMapLoader.cpp)

add_library(sd-tilemap ${TileMap_Src})

target_compile_options(sd-tilemap PUBLIC -Wall -Wextra -Wundef -pedantic -std=c++17)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-tilemap
        INTERFACE
        SD_TILE_MAP_IMPORT

        PRIVATE
        SD_TILE_MAP_EXPORT)
endif()

target_link_libraries(sd-tilemap
    PUBLIC
    sd-graphics
    sd-renderer
    sd-asset)

target_include_directories(sd-tilemap
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include)

install(TARGETS sd-tilemap LIBRARY DESTINATION lib)

install(FILES ${TileMap_Include} DESTINATION include/TileMap)
