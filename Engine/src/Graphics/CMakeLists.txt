find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

set(Include_Root ${SD_ENGINE_SOURCE_DIR}/include/Graphics)
set(Src_Root ${SD_ENGINE_SOURCE_DIR}/src/Graphics)

set(OpenGL_Include
    ${Include_Root}/OpenGL/GLBuffer.hpp
    ${Include_Root}/OpenGL/GLDevice.hpp
    ${Include_Root}/OpenGL/GLFramebuffer.hpp
    ${Include_Root}/OpenGL/GLShader.hpp
    ${Include_Root}/OpenGL/GLTexture.hpp
    ${Include_Root}/OpenGL/GLTranslator.hpp
    ${Include_Root}/OpenGL/GLVertexArray.hpp)

set(OpenGL_Src
    ${Src_Root}/OpenGL/GLBuffer.cpp
    ${Src_Root}/OpenGL/GLDevice.cpp
    ${Src_Root}/OpenGL/GLFramebuffer.cpp
    ${Src_Root}/OpenGL/GLShader.cpp
    ${Src_Root}/OpenGL/GLTexture.cpp
    ${Src_Root}/OpenGL/GLTranslator.cpp
    ${Src_Root}/OpenGL/GLVertexArray.cpp)

set(Graphics_Include
    ${OpenGL_Include}
    ${Include_Root}/Buffer.hpp
    ${Include_Root}/BufferLayout.hpp
    ${Include_Root}/Camera.hpp
    ${Include_Root}/Device.hpp
    ${Include_Root}/Font.hpp
    ${Include_Root}/Framebuffer.hpp
    ${Include_Root}/Graphics.hpp
    ${Include_Root}/Light.hpp
    ${Include_Root}/Material.hpp
    ${Include_Root}/Mesh.hpp
    ${Include_Root}/Model.hpp
    ${Include_Root}/Renderer.hpp
    ${Include_Root}/RenderTarget.hpp
    ${Include_Root}/Shader.hpp
    ${Include_Root}/Terrain.hpp
    ${Include_Root}/Texture.hpp
    ${Include_Root}/Translator.hpp
    ${Include_Root}/VertexArray.hpp)

set(Graphics_Src
    ${OpenGL_Src}
    ${Src_Root}/Buffer.cpp
    ${Src_Root}/BufferLayout.cpp
    ${Src_Root}/Camera.cpp
    ${Src_Root}/Device.cpp
    ${Src_Root}/Font.cpp
    ${Src_Root}/Framebuffer.cpp
    ${Src_Root}/Graphics.cpp
    ${Src_Root}/Light.cpp
    ${Src_Root}/Material.cpp
    ${Src_Root}/Mesh.cpp
    ${Src_Root}/Model.cpp
    ${Src_Root}/Renderer.cpp
    ${Src_Root}/RenderTarget.cpp
    ${Src_Root}/Shader.cpp
    ${Src_Root}/Terrain.cpp
    ${Src_Root}/Texture.cpp
    ${Src_Root}/VertexArray.cpp)

add_library(sd-graphics ${Graphics_Src})

set_property(TARGET sd-graphics PROPERTY CXX_STANDARD 17)

target_compile_options(sd-graphics PUBLIC -Wall -Wextra -Wundef -pedantic)

target_compile_definitions(sd-graphics PUBLIC SD_EXPORT)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(sd-graphics PUBLIC SD_SHARED)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(sd-graphics PUBLIC X11 Xrandr Xi)
endif()

target_link_libraries(sd-graphics PUBLIC sd-utils GLEW::GLEW GL)

target_include_directories(sd-graphics
    PUBLIC
    ${SD_ENGINE_SOURCE_DIR}/include)

install(TARGETS sd-graphics LIBRARY DESTINATION lib)

install(FILES ${Graphics_Include} DESTINATION include/Graphics)
