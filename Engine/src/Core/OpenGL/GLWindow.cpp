#include "Core/OpenGL/GLWindow.hpp"
#include "Core/SDL.hpp"
#include <GL/glew.h>

namespace SD {

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
    if (property.msaa > 1) {
        SDL(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1));
        SDL(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, property.msaa));
    }

    m_window = SDL_CreateWindow(property.title.c_str(), property.x, property.y,
                                property.width, property.height, sdlFlags);
    SD_CORE_ASSERT(m_window != nullptr, SDL_GetError());

    m_context = SDL_GL_CreateContext(m_window);
    SD_CORE_ASSERT(m_context != nullptr, SDL_GetError());

    SDL(SDL_GL_SetSwapInterval(property.vsync ? 1 : 0));

    SD_CORE_ASSERT(glewInit() == GLEW_OK, "glewInit failed!");
}

GLWindow::~GLWindow() { SDL_GL_DeleteContext(m_context); }

void *GLWindow::GetGraphicsContext() { return m_context; }

uint8_t GLWindow::GetMSAA() const {
    int value = 1;
    SDL(SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &value));
    return value;
}

bool GLWindow::GetIsVSync() const { return SDL_GL_GetSwapInterval() == 1; }

glm::ivec2 GLWindow::GetSize() const {
    glm::ivec2 size;
    SDL_GetWindowSize(m_window, &size.x, &size.y);
    return size;
}

int GLWindow::GetWidth() const {
    int width = 0;
    SDL_GetWindowSize(m_window, &width, nullptr);
    return width;
}

int GLWindow::GetHeight() const {
    int height = 0;
    SDL_GetWindowSize(m_window, nullptr, &height);
    return height;
}

void *GLWindow::GetHandle() const { return static_cast<void *>(m_window); }

std::string GLWindow::GetTitle() const { return SDL_GetWindowTitle(m_window); }

void GLWindow::SwapBuffer() { SDL_GL_SwapWindow(m_window); }

}  // namespace SD
