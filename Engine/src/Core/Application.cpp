#include "Core/Application.hpp"
#include "Core/SDL.hpp"
#include "Core/Input.hpp"
#include "Utility/Timing.hpp"
#include "Utility/Random.hpp"

#include "Renderer/BitmapLoader.hpp"
#include "Renderer/ModelLoader.hpp"
#include "Renderer/FontLoader.hpp"

#include "Renderer/Model.hpp"
#include "Renderer/Bitmap.hpp"
#include "Renderer/Font.hpp"

namespace SD {

const std::string setting_filename = "setting.ini";

Application::Application(const std::string &title) {
    WindowProp property;

    std::string debug_path = (GetAppDirectory() / "Debug.txt").string();
    Log::Init(debug_path);
    SD_CORE_INFO("Debug info is output to: {}", debug_path);

    ini = CreateRef<Ini>();
    std::filesystem::path ini_path = GetAppDirectory() / setting_filename;
    if (std::filesystem::exists(ini_path)) {
        ini->Load(ini_path.string());
    } else {
        SD_CORE_WARN(
            "No such ini file: {}. The application will create a new one.",
            ini_path);
    }

    property.title = title;
    property.width = ini->GetInteger("window", "width", property.width);
    property.height = ini->GetInteger("window", "height", property.height);
    property.msaa = ini->GetInteger("window", "msaa", property.msaa);
    property.vsync = ini->GetBoolean("window", "vsync", property.vsync);

    SDL(SDL_Init(SDL_INIT_EVERYTHING));

    // Setting up which api to use
    SetGraphicsAPI(GraphicsAPI::OpenGL);

    ShaderLibrary::Instance().SetDirectory("assets");

    window = Window::Create(property);

    asset = CreateRef<AssetManager>();
    asset->SetLoader<Bitmap>(new BitmapLoader(*asset));
    asset->SetLoader<Model>(new ModelLoader(*asset));
    asset->SetLoader<Font>(new FontLoader(*asset));
    asset->Load("assets");

    renderer =
        CreateRef<Renderer>(property.width, property.width, property.msaa);
    dispatcher = CreateRef<EventDispatcher>();
    scene = CreateRef<Scene>();

    m_imguiLayer = CreateLayer<ImGuiLayer>();
    PushOverlay(m_imguiLayer);
}

Application::~Application() {
    asset->Save();

    glm::ivec2 size = window->GetSize();
    ini->SetInteger("window", "width", size.x);
    ini->SetInteger("window", "height", size.y);
    ini->SetInteger("window", "msaa", window->GetMSAA());
    ini->SetBoolean("window", "vsync", window->GetIsVSync());

    ini->Save((GetAppDirectory() / setting_filename).string());

    while (m_layers.Size()) {
        DestroyLayer(m_layers.Front());
    }
    SDL_Quit();
}

void Application::PushLayer(Layer *layer) {
    layer->OnPush();
    m_layers.Push(layer);
}

void Application::PushOverlay(Layer *layer) {
    layer->OnPush();
    m_layers.PushOverlay(layer);
}

void Application::PopLayer(Layer *layer) {
    layer->OnPop();
    m_layers.Pop(layer);
}

void Application::DestroyLayer(Layer *layer) {
    if (m_layers.Has(layer)) {
        PopLayer(layer);
    }
    delete layer;
}

void Application::ProcessEvent(const SDL_Event &sdl_event) {
    if (sdl_event.type == SDL_QUIT) {
        Quit();
    }
    Event event;
    switch (sdl_event.type) {
        case SDL_MOUSEMOTION: {
            event.type = EventType::MOUSE_MOTION;
            event.mouse_motion.x = sdl_event.motion.x;
            event.mouse_motion.y = sdl_event.motion.y;
            event.mouse_motion.x_rel = sdl_event.motion.xrel;
            event.mouse_motion.y_rel = sdl_event.motion.yrel;
            Input::SetMouseCoord(event.mouse_motion.x, event.mouse_motion.y);
        } break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP: {
            event.type = EventType::MOUSE_BUTTON;
            event.mouse_button.button =
                static_cast<MouseButton>(sdl_event.button.button);
            event.mouse_button.x = sdl_event.button.x;
            event.mouse_button.y = sdl_event.button.y;
            event.mouse_button.clicks = sdl_event.button.clicks;
            event.mouse_button.state = sdl_event.button.state;
            Input::SetMouseButtonState(event.mouse_button.button,
                                       event.mouse_button.state);
        } break;
        case SDL_MOUSEWHEEL: {
            event.type = EventType::MOUSE_WHEEL_SCROLLED;
            event.mouse_wheel.x = sdl_event.wheel.x;
            event.mouse_wheel.y = sdl_event.wheel.y;
        } break;
        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            event.type = EventType::KEY;
            event.key.keycode = static_cast<Keycode>(sdl_event.key.keysym.sym);
            event.key.mod = sdl_event.key.keysym.mod;
            event.key.state = sdl_event.key.state;
            Input::SetKeyState(event.key.keycode, event.key.state);
        } break;
        case SDL_WINDOWEVENT: {
            switch (sdl_event.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_SIZE_CHANGED: {
                    event.type = EventType::WINDOW_RESIZED;
                    event.window_size.width = sdl_event.window.data1;
                    event.window_size.height = sdl_event.window.data2;
                } break;
            }
        } break;
        case SDL_TEXTINPUT: {
            event.type = EventType::TEXT_INPUT;
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
    while (!window->ShouldClose()) {
        while (window->PollEvent(event)) {
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

void Application::Quit() { window->SetShouldClose(true); }

void Application::Tick(float dt) {
    for (auto iter = m_layers.rbegin(); iter != m_layers.rend(); ++iter) {
        (*iter)->OnTick(dt);
    }
    Input::Tick();
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

    window->SwapBuffer();
}

}  // namespace SD
