#include "Core/GLContext.hpp"
#include "Core/Window.hpp"
#include "Core/Log.hpp"
#include "GL/glew.h"

namespace sd {

bool GLContext::create(SDL_Window *window) {
    m_context = SDL_GL_CreateContext(window);
    if (m_context == nullptr) {
        SD_CORE_ERROR("SDL_GL_CreateContext failed: {}", SDL_GetError());
        return false;
    }
    if (glewInit() != GLEW_OK) {
        SD_CORE_ERROR("glewInit failed!");
    }
    return true;
}

void GLContext::destroy() { SDL_GL_DeleteContext(m_context); }

GLContext::~GLContext() { destroy(); }

}  // namespace sd
