#include "Core/OpenGL/GLWindow.hpp"
#include "Core/Event.hpp"
#include "Core/Layer.hpp"
#include "Core/Input.hpp"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

#include <SDL.h>

#ifdef DEBUG_BUILD
#define SDL(stmt) SD_CORE_ASSERT(stmt == 0, SDL_GetError())
#else
#define SDL(stmt) stmt
#endif

namespace SD {

GLWindow::GLWindow(const WindowCreateInfo &info) : m_is_init_imgui(false)
{
    SDL(SDL_Init(SDL_INIT_EVERYTHING));
    uint32_t sdl_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;

    // Double buffer
    SDL(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1));

    // OpenGL Version
    SDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4));
    SDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5));
    SDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE));

    // MultiSampling
    if (info.msaa != MultiSampleLevel::None) {
        SDL(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1));
        SDL(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,
                                static_cast<int>(info.msaa)));
    }

    m_window = SDL_CreateWindow(info.title.c_str(), SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, info.width, info.height,
                                sdl_flags);
    SD_CORE_ASSERT(m_window != nullptr, SDL_GetError());

    m_context = SDL_GL_CreateContext(m_window);
    SD_CORE_ASSERT(m_context != nullptr, SDL_GetError());

    SDL(SDL_GL_SetSwapInterval(info.vsync ? 1 : 0));
}

GLWindow::~GLWindow()
{
    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

template <typename T>
void ProcessEvent(T &e, EventStack<Layer *> &layers)
{
    for (auto iter = layers.rbegin(); iter != layers.rend(); ++iter) {
        (*iter)->On(e);
    }
}

void GLWindow::PollEvents(EventStack<Layer *> &layers)
{
    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event) == 1) {
        if (m_is_init_imgui) {
            ImGui_ImplSDL2_ProcessEvent(&sdl_event);
        }
        switch (sdl_event.type) {
            case SDL_MOUSEMOTION: {
                MouseMotionEvent event;
                event.x = sdl_event.motion.x;
                event.y = sdl_event.motion.y;
                event.x_rel = sdl_event.motion.xrel;
                event.y_rel = sdl_event.motion.yrel;
                ProcessEvent(event, layers);
                Input::SetMouseCoord(event.x, event.y);
            } break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP: {
                MouseButtonEvent event;
                event.button =
                    static_cast<MouseButton>(sdl_event.button.button);
                event.x = sdl_event.button.x;
                event.y = sdl_event.button.y;
                event.clicks = sdl_event.button.clicks;
                event.state = sdl_event.button.state;
                ProcessEvent(event, layers);
                Input::SetMouseCoord(event.x, event.y);
                Input::SetMouseButtonState(event.button, event.state);
            } break;
            case SDL_MOUSEWHEEL: {
                MouseWheelEvent event;
                event.x = sdl_event.wheel.x;
                event.y = sdl_event.wheel.y;

                ProcessEvent(event, layers);
            } break;
            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                KeyEvent event;
                event.keycode = static_cast<Keycode>(sdl_event.key.keysym.sym);
                event.mod = sdl_event.key.keysym.mod;
                event.state = sdl_event.key.state;
                ProcessEvent(event, layers);

                Input::SetKeyState(event.keycode, event.state);
            } break;
            case SDL_WINDOWEVENT: {
                switch (sdl_event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED: {
                        WindowSizeEvent event;
                        event.width = sdl_event.window.data1;
                        event.height = sdl_event.window.data2;
                        ProcessEvent(event, layers);
                    } break;
                }
            } break;
            case SDL_TEXTINPUT: {
                TextInputEvent event;
                std::copy(std::begin(sdl_event.text.text),
                          std::end(sdl_event.text.text), event.text);

                ProcessEvent(event, layers);
            } break;
            case SDL_QUIT: {
                AppQuitEvent event;
                ProcessEvent(event, layers);
            } break;
        }
    }
}

void GLWindow::ImGuiInitBackend()
{
    ImGui_ImplSDL2_InitForOpenGL(m_window, m_context);
    ImGui_ImplOpenGL3_Init("#version 450");
    m_is_init_imgui = true;
}

void GLWindow::ImGuiShutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    m_is_init_imgui = false;
}

void GLWindow::ImGuiNewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
}

void GLWindow::ImGuiRenderDrawData()
{
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

MultiSampleLevel GLWindow::GetMSAA() const
{
    int value = 0;
    SDL(SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &value));
    return static_cast<MultiSampleLevel>(value);
}

bool GLWindow::GetIsVSync() const { return SDL_GL_GetSwapInterval() == 1; }

Vector2i GLWindow::GetSize() const
{
    Vector2i size;
    SDL_GetWindowSize(m_window, &size.x, &size.y);
    return size;
}

int GLWindow::GetWidth() const
{
    int width = 0;
    SDL_GetWindowSize(m_window, &width, nullptr);
    return width;
}

int GLWindow::GetHeight() const
{
    int height = 0;
    SDL_GetWindowSize(m_window, nullptr, &height);
    return height;
}

std::string GLWindow::GetTitle() const { return SDL_GetWindowTitle(m_window); }

void GLWindow::SwapBuffer() { SDL_GL_SwapWindow(m_window); }

}  // namespace SD
