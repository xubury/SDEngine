set(Include_Root ${PROJECT_SOURCE_DIR}/include/Renderer)
set(Src_Root ${PROJECT_SOURCE_DIR}/src/Renderer)

set(Renderer_Include
    ${Include_Root}/Renderer.hpp
    ${Include_Root}/Renderer2D.hpp
    ${Include_Root}/Renderer3D.hpp)

set(Renderer_Src
    ${Src_Root}/Renderer.cpp
    ${Src_Root}/Renderer2D.cpp
    ${Src_Root}/Renderer3D.cpp)

add_library(sd-renderer ${Renderer_Src})

set_property(TARGET sd-renderer PROPERTY CXX_STANDARD 17)

target_compile_options(sd-renderer PUBLIC -Wall -Wextra -Wundef -pedantic)

target_compile_definitions(sd-renderer PUBLIC SD_EXPORT)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-renderer PUBLIC SD_SHARED)
endif()

target_link_libraries(sd-renderer PUBLIC sd-graphics)

target_include_directories(sd-renderer
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include)


install(TARGETS sd-renderer LIBRARY DESTINATION lib)

install(FILES ${Renderer_Include} DESTINATION include/Renderer)
