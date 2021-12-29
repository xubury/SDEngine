set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/TileMap)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/TileMap)

set(TileMap_Include
    ${Include_Root}/TileLayout.hpp
    ${Include_Root}/Tile.hpp)

set(TileMap_Src)

add_library(sd-tilemap INTERFACE)

target_compile_options(sd-tilemap INTERFACE -Wall -Wextra -Wundef -pedantic -std=c++17)

target_link_libraries(sd-tilemap INTERFACE sd-renderer)

target_include_directories(sd-tilemap
    INTERFACE
    ${SD_ENGINE_SOURCE_DIR}/include)

install(TARGETS sd-tilemap LIBRARY DESTINATION lib)

install(FILES ${TileMap_Include} DESTINATION include/TileMap)
