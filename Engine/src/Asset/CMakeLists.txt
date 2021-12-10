set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/Asset)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/Asset)

set(Asset_Include
    ${Include_Root}/Export.hpp
    ${Include_Root}/Asset.hpp)

set(Asset_Src
    ${Src_Root}/Asset.cpp)

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
    ${SD_ENGINE_SOURCE_DIR}/include)

target_link_libraries(sd-asset
    PUBLIC
    sd-utility)

install(TARGETS sd-asset LIBRARY DESTINATION lib)

install(FILES ${Asset_Include} DESTINATION include/Asset)
