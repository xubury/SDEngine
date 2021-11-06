find_package(SDL2 REQUIRED)

set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/Input)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/Input)

set(Input_Include
    ${Include_Root}/Action.hpp
    ${Include_Root}/ActionMap.hpp
    ${Include_Root}/ActionTarget.hpp
    ${Include_Root}/InputEngine.hpp)

set(Input_Src
    ${Src_Root}/Action.cpp
    ${Src_Root}/InputEngine.cpp)

add_library(sd-input ${Input_Src})

set_property(TARGET sd-input PROPERTY CXX_STANDARD 17)

target_compile_options(sd-input PUBLIC -Wall -Wextra -Wundef -pedantic)

target_compile_definitions(sd-input PUBLIC SD_EXPORT)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-input PUBLIC SD_SHARED)
endif()

target_link_libraries(sd-input PUBLIC sd-graphics sd-system SDL2::Main)

target_include_directories(sd-input
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include)

install(TARGETS sd-input LIBRARY DESTINATION lib)

install(FILES ${Input_Include} DESTINATION include/Input)
