#include "Core/Application.hpp"
#include "Core/SDL.hpp"
#include "Utility/Timing.hpp"
#include "Utility/Random.hpp"
#include "Input/Input.hpp"

namespace SD {

static Application *s_instance = nullptr;

Application &GetApp() { return *s_instance; }

Window &Application::GetWindow() { return *m_window; }

Application::Application() {
    s_instance = this;

    std::string debug_path = "Debug.txt";
    Log::Init(debug_path);
    int width = 1600;
    int height = 900;
    int samples = 4;

    Random::Init();
    SD_CORE_INFO("Debug info is output to: {}", debug_path);

    SDL(SDL_Init(SDL_INIT_EVERYTHING));
    int img_flags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF;
    SD_CORE_ASSERT((IMG_Init(img_flags) & img_flags) == img_flags,
                   IMG_GetError());

    // Setting up which api to use
    SetGraphicsAPI(GraphicsAPI::OpenGL);

    WindowProp prop;
    prop.width = width;
    prop.height = height;
    prop.samples = samples;
    prop.flag = SDL_WINDOW_MAXIMIZED;
    m_window = Window::Create(prop);

    ShaderLibrary::Instance().SetRootPath("assets");

    renderer = CreateRef<Renderer>(samples);
    asset = CreateRef<AssetManager>("assets");
    dispatcher = CreateRef<EventDispatcher>();

    m_imguiLayer = new ImGuiLayer();
    PushOverlay(m_imguiLayer);
}

Application::~Application() {
    while (m_layers.size()) {
        DestroyLayer(m_layers.front());
    }
    IMG_Quit();
    SDL_Quit();
}

void Application::PushLayer(Layer *layer) {
    layer->SetAppVars(MakeAppVars());
    layer->OnPush();
    m_layers.Push(layer);
}

void Application::PushOverlay(Layer *layer) {
    layer->SetAppVars(MakeAppVars());
    layer->OnPush();
    m_layers.PushOverlay(layer);
}

void Application::PopLayer(Layer *layer) {
    layer->OnPop();
    m_layers.Pop(layer);
}

void Application::DestroyLayer(Layer *layer) {
    PopLayer(layer);
    delete layer;
}

void Application::ProcessEvent(const SDL_Event &sdl_event) {
    if (sdl_event.type == SDL_QUIT) {
        Quit();
    }
    Event event;
    switch (sdl_event.type) {
        case SDL_MOUSEMOTION:
            event.type = Event::EventType::MOUSE_MOTION;
            event.mouseMotion.x = sdl_event.motion.x;
            event.mouseMotion.y = sdl_event.motion.y;
            event.mouseMotion.xrel = sdl_event.motion.xrel;
            event.mouseMotion.yrel = sdl_event.motion.yrel;
            Input::SetMouseCoord(event.mouseMotion.x, event.mouseMotion.y);
            break;
        case SDL_MOUSEBUTTONDOWN:
            event.type = Event::EventType::MOUSE_BUTTON_PRESSED;
            event.mouseButton.button =
                static_cast<MouseButton>(sdl_event.button.button);
            SDL_BUTTON_LEFT;
            event.mouseButton.x = sdl_event.button.x;
            event.mouseButton.y = sdl_event.button.y;
            event.mouseButton.clicks = sdl_event.button.clicks;
            Input::PressMouseButton(event.mouseButton.button);
            break;
        case SDL_MOUSEBUTTONUP:
            event.type = Event::EventType::MOUSE_BUTTON_RELEASED;
            event.mouseButton.button =
                static_cast<MouseButton>(sdl_event.button.button);
            event.mouseButton.x = sdl_event.button.x;
            event.mouseButton.y = sdl_event.button.y;
            event.mouseButton.clicks = sdl_event.button.clicks;
            Input::ReleaseMouseButton(event.mouseButton.button);
            break;
        case SDL_MOUSEWHEEL:
            event.type = Event::EventType::MOUSE_WHEEL_SCROLLED;
            event.mouseWheel.x = sdl_event.wheel.x;
            event.mouseWheel.y = sdl_event.wheel.y;
            break;
        case SDL_KEYDOWN:
            event.type = Event::EventType::KEY_PRESSED;
            event.key.keycode = static_cast<Keycode>(sdl_event.key.keysym.sym);
            event.key.mod = sdl_event.key.keysym.mod;
            Input::PressKey(event.key.keycode);
            break;
        case SDL_KEYUP:
            event.type = Event::EventType::KEY_RELEASED;
            event.key.keycode = static_cast<Keycode>(sdl_event.key.keysym.sym);
            event.key.mod = sdl_event.key.keysym.mod;
            Input::ReleaseKey(event.key.keycode);
            break;
        case SDL_WINDOWEVENT:
            switch (sdl_event.window.type) {
                case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_SIZE_CHANGED: {
                    event.type = Event::EventType::WINDOW_RESIZED;
                    event.size.width = sdl_event.window.data1;
                    event.size.height = sdl_event.window.data2;
                } break;
            }
            break;
        case SDL_TEXTINPUT: {
            event.type = Event::EventType::TEXT_INPUT;
            std::copy(std::begin(sdl_event.text.text),
                      std::end(sdl_event.text.text), event.text.text);
        } break;
    }
    for (auto layer = m_layers.rbegin(); layer != m_layers.rend(); ++layer) {
        (*layer)->OnEventProcess(event);
        if ((*layer)->IsBlockEvent()) break;
    }
}

void Application::ProcessEvents() {
    for (auto layer = m_layers.rbegin(); layer != m_layers.rend(); ++layer) {
        (*layer)->OnEventsProcess();
        if ((*layer)->IsBlockEvent()) break;
    }
}

void Application::Run() {
    Clock clock;
    float min_fps = 30;
    SDL_Event event;
    float ms_per_frame = 1000.f / min_fps;
    uint32_t ms_elapsed = 0;
    while (!m_window->ShouldClose()) {
        while (m_window->PollEvent(event)) {
            ProcessEvent(event);
        }
        ProcessEvents();

        ms_elapsed = clock.Restart();
        while (ms_elapsed > ms_per_frame) {
            ms_elapsed -= ms_per_frame;
            Tick(ms_per_frame * 1e-3);
        }
        Tick(ms_elapsed * 1e-3);

        Render();
    }
}

void Application::Quit() { m_window->SetShouldClose(true); }

void Application::Tick(float dt) {
    Input::Tick();

    for (auto iter = m_layers.rbegin(); iter != m_layers.rend(); ++iter) {
        (*iter)->OnTick(dt);
    }
}

void Application::Render() {
    for (auto &layer : m_layers) {
        layer->OnRender();
    }

    m_imguiLayer->Begin();
    for (auto &layer : m_layers) {
        layer->OnImGui();
    }
    m_imguiLayer->End();

    m_window->SwapBuffer();
}

}  // namespace SD
