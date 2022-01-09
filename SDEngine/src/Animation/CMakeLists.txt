set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/Animation)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/Animation)

set(Animation_Include
    ${Include_Root}/Export.hpp
    ${Include_Root}/Animation.hpp
    ${Include_Root}/Animator.hpp)

set(Animation_Src
    ${Src_Root}/Animator.cpp)

add_library(sd-animation ${Animation_Src})

target_compile_options(sd-animation PUBLIC -Wall -Wextra -Wundef -pedantic -std=c++17)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-animation 
        INTERFACE
        SD_ANIMATION_IMPORT

        PRIVATE
        SD_ANIMATION_EXPORT)
endif()

target_include_directories(sd-animation
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include)

target_link_libraries(sd-animation
    PUBLIC
    sd-utility)

install(TARGETS sd-animation LIBRARY DESTINATION lib)

install(FILES ${Animation_Include} DESTINATION include/Animation)
