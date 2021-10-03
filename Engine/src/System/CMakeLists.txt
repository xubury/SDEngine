set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/System)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/System)

set(System_Include
    ${Include_Root}/ParticleSystem.hpp
    ${Include_Root}/ProfileSystem.hpp
    ${Include_Root}/RenderSystem.hpp
    ${Include_Root}/TerrainSystem.hpp)

set(System_Src
    ${Src_Root}/ParticleSystem.cpp
    ${Src_Root}/ProfileSystem.cpp
    ${Src_Root}/RenderSystem.cpp
    ${Src_Root}/TerrainSystem.cpp)


add_library(sd-system ${System_Src})

set_property(TARGET sd-system PROPERTY CXX_STANDARD 17)

target_compile_options(sd-system PUBLIC -Wall -Wextra -Wundef -pedantic)

target_compile_definitions(sd-system PUBLIC SD_EXPORT)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-system PUBLIC SD_SHARED)
endif()

target_include_directories(sd-system
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include)

target_link_libraries(sd-system PUBLIC sd-core sd-ecs sd-renderer)

install(TARGETS sd-system LIBRARY DESTINATION lib)

install(FILES ${Graphics_Include} DESTINATION include/System)
