set(Include_Root ${PROJECT_SOURCE_DIR}/include/Graphics)
set(Src_Root ${PROJECT_SOURCE_DIR}/src/Graphics)


set(Graphics_Include
    ${Include_Root}/Shader.hpp
    ${Include_Root}/BufferLayout.hpp
    ${Include_Root}/Buffer.hpp
    ${Include_Root}/VertexArray.hpp
    ${Include_Root}/Camera.hpp
    ${Include_Root}/Renderer.hpp
    ${Include_Root}/Renderer2D.hpp)

set(Graphics_Src
    ${Src_Root}/Shader.cpp
    ${Src_Root}/BufferLayout.cpp
    ${Src_Root}/Buffer.cpp
    ${Src_Root}/VertexArray.cpp
    ${Src_Root}/Camera.cpp
    ${Src_Root}/Renderer.cpp
    ${Src_Root}/Renderer2D.cpp)

add_library(sd-graphics ${Graphics_Src})

set_property(TARGET sd-graphics PROPERTY CXX_STANDARD 17)

target_compile_options(sd-graphics PUBLIC -Wall -Wextra -Wundef -pedantic)

target_compile_definitions(sd-graphics PUBLIC SD_EXPORT)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-graphics PUBLIC SD_SHARED)
endif()

target_link_libraries(sd-graphics PUBLIC sd-core)

target_include_directories(sd-graphics PRIVATE
    ${PROJECT_SOURCE_DIR}/libs)

target_include_directories(sd-graphics PUBLIC
    ${PROJECT_SOURCE_DIR}/include)


install(TARGETS sd-graphics LIBRARY DESTINATION lib)

install(FILES ${Graphics_Include} DESTINATION include/Graphics)
