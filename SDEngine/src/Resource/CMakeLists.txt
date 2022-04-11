set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/Resource)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/Resource)

set(Resource_Include
    ${Include_Root}/Export.hpp
    ${Include_Root}/Resource.hpp
    ${Include_Root}/ResourceId.hpp
    ${Include_Root}/ResourceManager.hpp)

set(Resource_Src
    ${Src_Root}/ResourceManager.cpp
    ${Src_Root}/ResourceId.cpp)

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
    ${SD_ENGINE_SOURCE_DIR}/include)

target_link_libraries(sd-resource PUBLIC sd-utility)

install(TARGETS sd-resource LIBRARY DESTINATION lib)

install(FILES ${Resource_Include} DESTINATION include/Resource)
