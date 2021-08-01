#include "Core/GLContext.hpp"
#include "Core/Window.hpp"
#include "Core/Log.hpp"
#include "Core/Assert.hpp"
#include "Graphics/Device.hpp"
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
    m_context = SDL_GL_CreateContext(window);
    if (m_context == nullptr) {
        SD_CORE_ERROR("SDL_GL_CreateContext failed: {}", SDL_GetError());
        return false;
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
    RendererAPI::init(API::OpenGL);
    // Blend
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return true;
}

void GLContext::destroy() { SDL_GL_DeleteContext(m_context); }

GLContext::~GLContext() { destroy(); }

}  // namespace sd
