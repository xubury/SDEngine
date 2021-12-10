find_package(glm REQUIRED)
find_package(spdlog REQUIRED)
find_package(cereal REQUIRED)

set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/Utility)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/Utility)


set(Utility_Include
    ${Include_Root}/Assert.hpp
    ${Include_Root}/Base.hpp
    ${Include_Root}/BlockingQueue.hpp
    ${Include_Root}/Config.hpp
    ${Include_Root}/Export.hpp
    ${Include_Root}/Exception.hpp
    ${Include_Root}/EventDispatcher.hpp
    ${Include_Root}/EventStack.hpp
    ${Include_Root}/File.hpp
    ${Include_Root}/Ini.hpp
    ${Include_Root}/Log.hpp
    ${Include_Root}/Math.hpp
    ${Include_Root}/PlatformDetection.hpp
    ${Include_Root}/QuadTree.hpp
    ${Include_Root}/Random.hpp
    ${Include_Root}/ResourceID.hpp
    ${Include_Root}/Serialize.hpp
    ${Include_Root}/String.hpp
    ${Include_Root}/Timing.hpp
    ${Include_Root}/Transform.hpp
    ${Include_Root}/ThreadPool.hpp)


set(Utility_Src
    ${Src_Root}/File.cpp
    ${Src_Root}/Ini.cpp
    ${Src_Root}/Log.cpp
    ${Src_Root}/Math.cpp
    ${Src_Root}/QuadTree.cpp
    ${Src_Root}/Random.cpp
    ${Src_Root}/ResourceId.cpp
    ${Src_Root}/Timing.cpp
    ${Src_Root}/Transform.cpp
    ${Src_Root}/ThreadPool.cpp)


add_library(sd-utility ${Utility_Src})

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_options(sd-utility PUBLIC -mconsole)
    else()
        target_link_options(sd-utility PUBLIC -mwindows)
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(sd-utility PUBLIC ${CMAKE_DL_LIBS} pthread)
endif()

target_compile_options(sd-utility PUBLIC -Wall -Wextra -Wundef -pedantic -std=c++17)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-utility 
        INTERFACE
        SD_UTILITY_IMPORT

        PRIVATE 
        SD_UTILITY_EXPORT)
endif()

target_include_directories(sd-utility
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include
    PRIVATE
    ${ASSIMP_INCLUDE_DIRS})

target_link_libraries(sd-utility
    PUBLIC
    spdlog::spdlog 
    glm 
    cereal)

install(TARGETS sd-utility LIBRARY DESTINATION lib)

install(FILES ${Utility_Include} DESTINATION include/Utility)
