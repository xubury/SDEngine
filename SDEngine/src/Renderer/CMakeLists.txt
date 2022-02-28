set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/Renderer)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/Renderer)


set(Renderer_Include
    ${Include_Root}/Export.hpp
    ${Include_Root}/Renderer.hpp
    ${Include_Root}/Bitmap.hpp)

set(Renderer_Src
    ${Src_Root}/Renderer.cpp
    ${Src_Root}/Bitmap.cpp)

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

    PRIVATE
    sd-loader)

target_include_directories(sd-renderer
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include
    PRIVATE
    ${SDL2_IMAGE_INCLUDE_DIRS})

install(TARGETS sd-renderer LIBRARY DESTINATION lib)

install(FILES ${Renderer_Include} DESTINATION include/Renderer)
