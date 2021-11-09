set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/Asset)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/Asset)

set(Asset_Include
    ${Include_Root}/Asset.hpp)


set(Asset_Src
    ${Src_Root}/Asset.cpp)


add_library(sd-asset ${Asset_Src})

set_property(TARGET sd-asset PROPERTY CXX_STANDARD 17)

target_compile_options(sd-asset PUBLIC -Wall -Wextra -Wundef -pedantic)

target_compile_definitions(sd-asset PUBLIC SD_EXPORT)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-asset PUBLIC SD_SHARED)
endif()

target_include_directories(sd-asset
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include)

# target_link_directories(sd-asset PRIVATE )

target_link_libraries(sd-asset PUBLIC sd-utils)

install(TARGETS sd-asset LIBRARY DESTINATION lib)

install(FILES ${Asset_Include} DESTINATION include/Asset)
