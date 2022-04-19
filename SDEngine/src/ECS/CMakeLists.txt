set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/ECS)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/ECS)

set(ECS_Include
    ${Include_Root}/Component.hpp
    ${Include_Root}/Export.hpp
    ${Include_Root}/Entity.hpp
    ${Include_Root}/Scene.hpp
    ${Include_Root}/SceneManager.hpp)

set(ECS_Src
    ${Src_Root}/Component.cpp
    ${Src_Root}/Entity.cpp
    ${Src_Root}/Scene.cpp
    ${Src_Root}/SceneManager.cpp)

add_library(sd-ecs ${ECS_Src})

target_compile_options(sd-ecs PUBLIC -Wall -Wextra -Wundef -pedantic -std=c++17)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-ecs
        INTERFACE
        SD_ECS_IMPORT

        PRIVATE
        SD_ECS_EXPORT)
endif()

target_include_directories(sd-ecs
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include
    PRIVATE
    ${SD_ENGINE_SOURCE_DIR}/libs/entt)

target_link_libraries(sd-ecs PUBLIC sd-utility EnTT::EnTT sd-graphics sd-animation)

install(TARGETS sd-ecs LIBRARY DESTINATION lib)

install(FILES ${ECS_Include} DESTINATION include/ECS)
