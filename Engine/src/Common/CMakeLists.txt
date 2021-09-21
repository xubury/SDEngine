find_package(spdlog REQUIRED)
find_package(cereal REQUIRED)
find_package(glm REQUIRED)

set(Include_Root ${PROJECT_SOURCE_DIR}/include/Common)
set(Src_Root ${PROJECT_SOURCE_DIR}/src/Common)

set(Common_Include
    ${Include_Root}/Assert.hpp
    ${Include_Root}/Base.hpp
    ${Include_Root}/Config.hpp
    ${Include_Root}/Export.hpp
    ${Include_Root}/PlatformDetection.hpp
    ${Include_Root}/Log.hpp)

set(Common_Src
    ${Src_Root}/Log.cpp)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_options(sd-core PUBLIC -mconsole)
else()
    target_link_options(sd-core PUBLIC -mwindows)
endif()

add_library(sd-common ${Common_Src})

set_property(TARGET sd-common PROPERTY CXX_STANDARD 17)

target_compile_options(sd-common PUBLIC -Wall -Wextra -Wundef -pedantic)

target_compile_definitions(sd-common PUBLIC SD_EXPORT)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-common PUBLIC SD_SHARED)
endif()

target_link_libraries(sd-common PUBLIC spdlog::spdlog glm)
add_dependencies(sd-common cereal)

target_include_directories(sd-common
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include)


install(TARGETS sd-common LIBRARY DESTINATION lib)

install(FILES ${Graphics_Include} DESTINATION include/Common)
