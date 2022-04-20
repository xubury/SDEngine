find_package(assimp REQUIRED)
find_package(Freetype REQUIRED)

set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/Resource)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/Resource)

set(Resource_Include
    ${Include_Root}/Export.hpp
    ${Include_Root}/Resource.hpp
    ${Include_Root}/TextureLoader.hpp
    ${Include_Root}/FontLoader.hpp
    ${Include_Root}/ModelLoader.hpp
    ${Include_Root}/ShaderLoader.hpp
    ${Include_Root}/SceneLoader.hpp
    ${Include_Root}/ImageLoader.hpp)

set(Resource_Src
    ${Src_Root}/TextureLoader.cpp
    ${Src_Root}/FontLoader.cpp
    ${Src_Root}/ModelLoader.cpp
    ${Src_Root}/ShaderLoader.cpp
    ${Src_Root}/SceneLoader.cpp
    ${Src_Root}/ImageLoader.cpp)

add_library(sd-resource ${Resource_Src})

target_compile_options(sd-resource PUBLIC -Wall -Wextra -Wundef -pedantic -std=c++17)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-resource
        INTERFACE
        SD_RESOURCE_IMPORT

        PRIVATE
        SD_RESOURCE_EXPORT)
endif()

target_include_directories(sd-resource
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include
    PRIVATE
    ${SD_ENGINE_SOURCE_DIR}/libs)

target_link_libraries(sd-resource
    PUBLIC
    sd-utility
    sd-graphics

    PRIVATE
    assimp::assimp
    Freetype::Freetype)

install(TARGETS sd-resource LIBRARY DESTINATION lib)

install(FILES ${Resource_Include} DESTINATION include/Resource)
