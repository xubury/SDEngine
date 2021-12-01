find_package(SDL2 REQUIRED)

set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/Asset)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/Asset)

set(Asset_Include
    ${Include_Root}/Asset.hpp
    ${Include_Root}/Image.hpp)

set(Asset_Src
    ${Src_Root}/Asset.cpp
    ${Src_Root}/Image.cpp)

add_library(sd-asset ${Asset_Src})

target_compile_options(sd-asset PUBLIC -Wall -Wextra -Wundef -pedantic -std=c++17)

target_compile_definitions(sd-asset PRIVATE SD_ASSET_EXPORT)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-asset PUBLIC SD_SHARED)
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
    SDL2::SDL2)

install(TARGETS sd-asset LIBRARY DESTINATION lib)

install(FILES ${Asset_Include} DESTINATION include/Asset)
