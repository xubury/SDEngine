set(Include_Root ${PROJECT_SOURCE_DIR}/include/Utils)
set(Src_Root ${PROJECT_SOURCE_DIR}/src/Utils)

set(Utils_Include
    ${Include_Root}/AssetLoader.hpp
    ${Include_Root}/AssetManager.hpp
    ${Include_Root}/File.hpp
    ${Include_Root}/Transform.hpp)

set(Utils_Src
    ${Src_Root}/File.cpp
    ${Src_Root}/Random.cpp
    ${Src_Root}/Transform.cpp)

add_library(sd-utils ${Utils_Src})

set_property(TARGET sd-utils PROPERTY CXX_STANDARD 17)

target_compile_options(sd-utils PUBLIC -Wall -Wextra -Wundef -pedantic)

target_compile_definitions(sd-utils PUBLIC SD_EXPORT)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-utils PUBLIC SD_SHARED)
endif()

target_include_directories(sd-utils
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(sd-utils PUBLIC sd-common)

install(TARGETS sd-utils LIBRARY DESTINATION lib)

install(FILES ${Graphics_Include} DESTINATION include/Utils)
