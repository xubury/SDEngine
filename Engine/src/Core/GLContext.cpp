#include "Core/GLContext.hpp"
#include "Core/Window.hpp"
#include "Utils/Assert.hpp"
#include <GL/glew.h>

namespace sd {
#ifdef DEBUG_BUILD
static void OpenGLMessageCallback(GLenum, GLenum, unsigned, GLenum severity,
                                  int, const char* message, const void*) {
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            SD_CORE_CRITICAL(message);
            return;
        case GL_DEBUG_SEVERITY_MEDIUM:
            SD_CORE_ERROR(message);
            return;
        case GL_DEBUG_SEVERITY_LOW:
            SD_CORE_WARN(message);
            return;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            SD_CORE_TRACE(message);
            return;
        default:
            SD_CORE_ASSERT(false, "Unknown severity level!");
    }
}
#endif

#ifdef DEBUG_BUILD
#define SDL(stmt) \
    if (stmt != 0) SD_CORE_ERROR(#stmt " Failed: {}", SDL_GetError())
#else
#define SDL(stmt) stmt
#endif

bool GLContext::create(SDL_Window* window) {
    SD_CORE_TRACE("Initializing GLContext...");
    m_context = SDL_GL_CreateContext(window);
    if (m_context == nullptr) {
        SD_CORE_ERROR("SDL_GL_CreateContext failed: {}", SDL_GetError());
        return false;
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
    SDL(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8));

    if (glewInit() != GLEW_OK) {
        SD_CORE_ERROR("glewInit failed!");
        return false;
    }
#ifdef DEBUG_BUILD
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OpenGLMessageCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
                          GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif
    // Depth Test
    glEnable(GL_DEPTH_TEST);

    // Blend
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // MultiSampling
    glEnable(GL_MULTISAMPLE);

    return true;
}

GLContext::~GLContext() { SDL_GL_DeleteContext(m_context); }

SDL_GLContext GLContext::getSDLHandle() const { return m_context; }

}  // namespace sd
