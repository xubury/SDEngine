#include "Core/OpenGL/GLContext.hpp"
#include "Core/SDL.hpp"
#include "Utility/Assert.hpp"
#include <GL/glew.h>

namespace sd {

GLContext::GLContext(const WindowProp& property, SDL_Window** window) {
    SD_CORE_TRACE("Initializing GLContext...");
    uint32_t sdlFlags =
        property.flag | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;

    // Double buffer
    SDL(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1));

    // OpenGL Version
    SDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4));
    SDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5));
    SDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE));

    // MultiSampling
    SDL(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1));
    SDL(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, property.samples));

    m_window = SDL_CreateWindow(property.title.c_str(), property.x, property.y,
                                property.width, property.height, sdlFlags);
    if (m_window == nullptr) {
        SD_CORE_ERROR("SDL_CreateWindow failed: {}", SDL_GetError());
        exit(-1);
    }

    m_context = SDL_GL_CreateContext(m_window);

    SDL(SDL_GL_SetSwapInterval(property.vsync ? 1 : 0));

    *window = m_window;
    if (m_context == nullptr) {
        SD_CORE_ERROR("SDL_GL_CreateContext failed: {}", SDL_GetError());
        exit(-1);
    }

    if (glewInit() != GLEW_OK) {
        SD_CORE_ERROR("glewInit failed!");
        exit(-1);
    }
}

GLContext::~GLContext() { SDL_GL_DeleteContext(m_context); }

void* GLContext::getHandle() const { return m_context; }

void GLContext::swapBuffer() const { SDL_GL_SwapWindow(m_window); }

}  // namespace sd
