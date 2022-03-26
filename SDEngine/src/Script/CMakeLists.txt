find_package(Lua REQUIRED)

set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/Script)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/Script)


set(Script_Include
    ${Include_Root}/Export.hpp
    ${Include_Root}/ScriptLayer.hpp)

set(Script_Src
    ${Src_Root}/ScriptLayer.cpp)

add_library(sd-script ${Script_Src})

target_compile_options(sd-script PUBLIC -Wall -Wextra -Wundef -pedantic -std=c++17)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-script
        INTERFACE
        SD_RENDERER_IMPORT

        PRIVATE
        SD_RENDERER_EXPORT)
endif()

target_link_libraries(sd-script
    PUBLIC
    sd-core
    sd-utility

    PRIVATE
    ${LUA_LIBRARIES}
    sol2)


target_include_directories(sd-script
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include
    PRIVATE
    ${SDL2_IMAGE_INCLUDE_DIRS})

install(TARGETS sd-script LIBRARY DESTINATION lib)

install(FILES ${Script_Include} DESTINATION include/Script)
