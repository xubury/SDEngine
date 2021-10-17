#include "Core/OpenGL/GLWindow.hpp"
#include "Core/SDL.hpp"
#include "Utility/Assert.hpp"
#include <GL/glew.h>

namespace sd {

GLWindow::GLWindow(const WindowProp &property) {
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
    SD_CORE_ASSERT(m_window != nullptr, SDL_GetError());

    m_context = SDL_GL_CreateContext(m_window);
    SD_CORE_ASSERT(m_context != nullptr, SDL_GetError());

    SDL(SDL_GL_SetSwapInterval(property.vsync ? 1 : 0));

    SD_CORE_ASSERT(glewInit() == GLEW_OK, "glewInit failed!");
}

GLWindow::~GLWindow() { SDL_GL_DeleteContext(m_context); }

void *GLWindow::getGraphicsContext() const { return m_context; }

}  // namespace sd
