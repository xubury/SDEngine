find_package(glm REQUIRED)
find_package(spdlog REQUIRED)
find_package(assimp REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(Freetype REQUIRED)
find_package(cereal REQUIRED)

set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/Utility)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/Utility)

set(Loader_Include
    ${Include_Root}/Loader/AssetLoader.hpp
    ${Include_Root}/Loader/FontLoader.hpp
    ${Include_Root}/Loader/ImageLoader.hpp
    ${Include_Root}/Loader/ModelLoader.hpp
    ${Include_Root}/Loader/ShaderLoader.hpp)

set(Loader_Src
    ${Src_Root}/Loader/FontLoader.cpp
    ${Src_Root}/Loader/ImageLoader.cpp
    ${Src_Root}/Loader/ModelLoader.cpp
    ${Src_Root}/Loader/ShaderLoader.cpp)

set(Utility_Include
    ${Loader_Include}
    ${Include_Root}/Assert.hpp
    ${Include_Root}/Base.hpp
    ${Include_Root}/Config.hpp
    ${Include_Root}/EventDispatcher.hpp
    ${Include_Root}/Export.hpp
    ${Include_Root}/File.hpp
    ${Include_Root}/Image.hpp
    ${Include_Root}/Log.hpp
    ${Include_Root}/Math.hpp
    ${Include_Root}/PlatformDetection.hpp
    ${Include_Root}/QuadTree.hpp
    ${Include_Root}/Random.hpp
    ${Include_Root}/SDLHelper.hpp
    ${Include_Root}/Serialize.hpp
    ${Include_Root}/String.hpp
    ${Include_Root}/Timing.hpp
    ${Include_Root}/Transform.hpp)

set(Utility_Src
    ${Loader_Src}
    ${Src_Root}/File.cpp
    ${Src_Root}/Image.cpp
    ${Src_Root}/Log.cpp
    ${Src_Root}/Math.cpp
    ${Src_Root}/QuadTree.cpp
    ${Src_Root}/Random.cpp
    ${Src_Root}/SDLHelper.cpp
    ${Src_Root}/Timing.cpp
    ${Src_Root}/Transform.cpp)


add_library(sd-utils ${Utility_Src})

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_options(sd-utils PUBLIC -mconsole)
    else()
        target_link_options(sd-utils PUBLIC -mwindows)
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(sd-utils PUBLIC ${CMAKE_DL_LIBS} pthread)
endif()

set_property(TARGET sd-utils PROPERTY CXX_STANDARD 17)

target_compile_options(sd-utils PUBLIC -Wall -Wextra -Wundef -pedantic)

target_compile_definitions(sd-utils PUBLIC SD_EXPORT)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-utils PUBLIC SD_SHARED)
endif()

target_include_directories(sd-utils
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include
    PRIVATE
    ${ASSIMP_INCLUDE_DIRS})

target_link_directories(sd-utils PRIVATE ${ASSIMP_LIBRARY_DIRS})

target_link_libraries(sd-utils
    PUBLIC
    spdlog::spdlog glm
    Freetype::Freetype
    PRIVATE
    ${ASSIMP_LIBRARIES} SDL2::Image)

install(TARGETS sd-utils LIBRARY DESTINATION lib)

install(FILES ${Graphics_Include} DESTINATION include/Utility)
