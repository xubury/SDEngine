#include "Core/OpenGL/GLWindow.hpp"
#include "Core/SDL.hpp"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include <GL/glew.h>

namespace SD {

GLWindow::GLWindow(const WindowCreateInfo &info) {
    uint32_t sdl_flags = info.flag | SDL_WINDOW_OPENGL;

    // Double buffer
    SDL(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1));

    // OpenGL Version
    SDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4));
    SDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5));
    SDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE));

    // MultiSampling
    int samples = static_cast<int>(info.msaa);
    if (samples > 1) {
        SDL(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1));
        SDL(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, samples));
    }

    m_window = SDL_CreateWindow(info.title.c_str(), info.x, info.y, info.width,
                                info.height, sdl_flags);
    SD_CORE_ASSERT(m_window != nullptr, SDL_GetError());

    m_context = SDL_GL_CreateContext(m_window);
    SD_CORE_ASSERT(m_context != nullptr, SDL_GetError());

    SDL(SDL_GL_SetSwapInterval(info.vsync ? 1 : 0));

    SD_CORE_ASSERT(glewInit() == GLEW_OK, "glewInit failed!");
}

GLWindow::~GLWindow() {
    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
}

void GLWindow::ImGuiInitBackend() {
    ImGui_ImplSDL2_InitForOpenGL(m_window, m_context);
    ImGui_ImplOpenGL3_Init("#version 450");
}

void GLWindow::ImGuiShutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
}

void GLWindow::ImGuiNewFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
}

void GLWindow::ImGuiRenderDrawData() {
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

MultiSampleLevel GLWindow::GetMSAA() const {
    int value = 1;
    SDL(SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &value));
    return static_cast<MultiSampleLevel>(value);
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

std::string GLWindow::GetTitle() const { return SDL_GetWindowTitle(m_window); }

void GLWindow::SwapBuffer() { SDL_GL_SwapWindow(m_window); }

}  // namespace SD
