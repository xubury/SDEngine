find_package(spdlog REQUIRED)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(GLEW REQUIRED)

set(Include_Root ${PROJECT_SOURCE_DIR}/include/Core)
set(Src_Root ${PROJECT_SOURCE_DIR}/src/Core)

set(Core_Include
    ${Include_Root}/PlatformDetection.hpp
    ${Include_Root}/Config.hpp
    ${Include_Root}/Export.hpp
    ${Include_Root}/Log.hpp
    ${Include_Root}/Assert.hpp
    ${Include_Root}/Base.hpp
    ${Include_Root}/Timing.hpp
    ${Include_Root}/Window.hpp
    ${Include_Root}/GLContext.hpp
    ${Include_Root}/Engine.hpp
    ${Include_Root}/InputManager.hpp)

set(Core_Src
    ${Src_Root}/Log.cpp
    ${Src_Root}/Timing.cpp
    ${Src_Root}/Window.cpp
    ${Src_Root}/GLContext.cpp
    ${Src_Root}/Engine.cpp
    ${Src_Root}/InputManager.cpp)

add_library(sd-core ${Core_Src})

set_property(TARGET sd-core PROPERTY CXX_STANDARD 17)

target_compile_options(sd-core PRIVATE -Wall -Wextra -Wundef -pedantic)

target_compile_definitions(sd-core PRIVATE SD_EXPORT)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-core PRIVATE SD_SHARED)
endif()

target_link_libraries(sd-core PUBLIC spdlog::spdlog SDL2::Main GLEW::GLEW)

target_include_directories(sd-core PUBLIC
    ${PROJECT_SOURCE_DIR}/include)

target_include_directories(sd-core PRIVATE
    ${PROJECT_SOURCE_DIR}/libs)

install(TARGETS sd-core LIBRARY DESTINATION lib)

install(FILES ${Core_Include} DESTINATION include/Core)
