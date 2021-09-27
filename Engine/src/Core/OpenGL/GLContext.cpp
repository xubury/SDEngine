#include "Core/OpenGL/GLContext.hpp"
#include "Utils/Assert.hpp"
#include <GL/glew.h>

namespace sd {

#ifdef DEBUG_BUILD
#define SDL(stmt) \
    if (stmt != 0) SD_CORE_ERROR(#stmt " Failed: {}", SDL_GetError())
#else
#define SDL(stmt) stmt
#endif

GLContext::GLContext(const WindowProp& property, SDL_Window** window) {
    SD_CORE_TRACE("Initializing GLContext...");
    uint32_t sdlFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    if (property.flag & INVISIBLE) {
        sdlFlags |= SDL_WINDOW_HIDDEN;
    }
    if (property.flag & FULLSCREEN) {
        sdlFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    if (property.flag & BORDERLESS) {
        sdlFlags |= SDL_WINDOW_BORDERLESS;
    }

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
    m_context = SDL_GL_CreateContext(m_window);
    *window = m_window;
    if (m_context == nullptr) {
        SD_CORE_ERROR("SDL_GL_CreateContext failed: {}", SDL_GetError());
    }

    if (glewInit() != GLEW_OK) {
        SD_CORE_ERROR("glewInit failed!");
        return;
    }
}

GLContext::~GLContext() { SDL_GL_DeleteContext(m_context); }

void* GLContext::getHandle() const { return m_context; }

void GLContext::swapBuffer() const { SDL_GL_SwapWindow(m_window); }

}  // namespace sd
