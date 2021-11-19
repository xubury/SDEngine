#include "Core/Application.hpp"
#include "Core/SDL.hpp"
#include "Utility/Timing.hpp"
#include "Utility/Random.hpp"
#include "Input/Input.hpp"

namespace SD {

const std::string setting_filename = "setting.ini";

Window &Application::GetWindow() { return *m_window; }

void Application::OnInit() {
    std::string debug_path = GetAppDirectory() / "Debug.txt";
    Log::Init(debug_path);
    SD_CORE_INFO("Debug info is output to: {}", debug_path);

    ini = CreateRef<Ini>();
    ini->Load(GetAppDirectory() / setting_filename);

    int width = ini->GetInteger("window", "width", 1600);
    int height = ini->GetInteger("window", "height", 900);
    int msaa = ini->GetInteger("window", "msaa", 4);

    Random::Init();

    SDL(SDL_Init(SDL_INIT_EVERYTHING));
    int img_flags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF;
    SD_CORE_ASSERT((IMG_Init(img_flags) & img_flags) == img_flags,
                   IMG_GetError());

    // Setting up which api to use
    SetGraphicsAPI(GraphicsAPI::OpenGL);

    WindowProp prop;
    prop.width = width;
    prop.height = height;
    prop.msaa = msaa;
    prop.flag = SDL_WINDOW_MAXIMIZED;
    m_window = Window::Create(prop);

    ShaderLibrary::Instance().SetRootPath("assets");

    renderer = CreateRef<Renderer>(msaa);
    asset = CreateRef<AssetManager>("assets");
    dispatcher = CreateRef<EventDispatcher>();

    m_imguiLayer = new ImGuiLayer();
    PushOverlay(m_imguiLayer);
}

void Application::OnDestroy() {
    glm::ivec2 size = m_window->GetSize();
    ini->SetInteger("window", "width", size.x);
    ini->SetInteger("window", "height", size.y);
    ini->SetInteger("window", "msaa", m_window->GetMSAA());

    ini->Save(GetAppDirectory() / setting_filename);

    while (m_layers.Size()) {
        DestroyLayer(m_layers.Front());
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
            event.mouse_motion.x = sdl_event.motion.x;
            event.mouse_motion.y = sdl_event.motion.y;
            event.mouse_motion.x_rel = sdl_event.motion.xrel;
            event.mouse_motion.y_rel = sdl_event.motion.yrel;
            Input::SetMouseCoord(event.mouse_motion.x, event.mouse_motion.y);
            break;
        case SDL_MOUSEBUTTONDOWN:
            event.type = Event::EventType::MOUSE_BUTTON_PRESSED;
            event.mouse_button.button =
                static_cast<MouseButton>(sdl_event.button.button);
            SDL_BUTTON_LEFT;
            event.mouse_button.x = sdl_event.button.x;
            event.mouse_button.y = sdl_event.button.y;
            event.mouse_button.clicks = sdl_event.button.clicks;
            Input::PressMouseButton(event.mouse_button.button);
            break;
        case SDL_MOUSEBUTTONUP:
            event.type = Event::EventType::MOUSE_BUTTON_RELEASED;
            event.mouse_button.button =
                static_cast<MouseButton>(sdl_event.button.button);
            event.mouse_button.x = sdl_event.button.x;
            event.mouse_button.y = sdl_event.button.y;
            event.mouse_button.clicks = sdl_event.button.clicks;
            Input::ReleaseMouseButton(event.mouse_button.button);
            break;
        case SDL_MOUSEWHEEL:
            event.type = Event::EventType::MOUSE_WHEEL_SCROLLED;
            event.mouse_wheel.x = sdl_event.wheel.x;
            event.mouse_wheel.y = sdl_event.wheel.y;
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
                    event.window_size.width = sdl_event.window.data1;
                    event.window_size.height = sdl_event.window.data2;
                } break;
            }
            break;
        case SDL_TEXTINPUT: {
            event.type = Event::EventType::TEXT_INPUT;
            std::copy(std::begin(sdl_event.text.text),
                      std::end(sdl_event.text.text), event.text_input.text);
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
