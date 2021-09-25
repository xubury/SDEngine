find_package(glm REQUIRED)
find_package(spdlog REQUIRED)

set(Include_Root ${PROJECT_SOURCE_DIR}/include/Utils)
set(Src_Root ${PROJECT_SOURCE_DIR}/src/Utils)

set(Utils_Include
    ${Include_Root}/Assert.hpp
    ${Include_Root}/AssetLoader.hpp
    ${Include_Root}/Base.hpp
    ${Include_Root}/Config.hpp
    ${Include_Root}/Export.hpp
    ${Include_Root}/File.hpp
    ${Include_Root}/Log.hpp
    ${Include_Root}/Math.hpp
    ${Include_Root}/PlatformDetection.hpp
    ${Include_Root}/QuadTree.hpp
    ${Include_Root}/Random.hpp
    ${Include_Root}/Serialize.hpp
    ${Include_Root}/Transform.hpp)

set(Utils_Src
    ${Src_Root}/File.cpp
    ${Src_Root}/Log.cpp
    ${Src_Root}/Math.cpp
    ${Src_Root}/QuadTree.cpp
    ${Src_Root}/Random.cpp
    ${Src_Root}/Transform.cpp)


add_library(sd-utils ${Utils_Src})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_options(sd-utils PUBLIC -mconsole)
else()
    target_link_options(sd-utils PUBLIC -mwindows)
endif()

set_property(TARGET sd-utils PROPERTY CXX_STANDARD 17)

target_compile_options(sd-utils PUBLIC -Wall -Wextra -Wundef -pedantic)

target_compile_definitions(sd-utils PUBLIC SD_EXPORT)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-utils PUBLIC SD_SHARED)
endif()

target_include_directories(sd-utils
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(sd-utils PUBLIC spdlog::spdlog glm)

install(TARGETS sd-utils LIBRARY DESTINATION lib)

install(FILES ${Graphics_Include} DESTINATION include/Utils)
