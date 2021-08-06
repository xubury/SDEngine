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

bool GLContext::create(SDL_Window* window) {
    SD_CORE_TRACE("Initializing GLContext...");
    m_context = SDL_GL_CreateContext(window);
    if (m_context == nullptr) {
        SD_CORE_ERROR("SDL_GL_CreateContext failed: {}", SDL_GetError());
        return false;
    }
    if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) != 0) {
        SD_CORE_ERROR("SDL_GL_SetAttribute Failed: {}", SDL_GetError());
        exit(-1);
    }
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
    // Blend
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return true;
}

GLContext::~GLContext() { SDL_GL_DeleteContext(m_context); }

}  // namespace sd
