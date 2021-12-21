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

const std::string SETTING_FILENAME = "setting.ini";

Application::Application(const std::string &title, GraphicsAPI api) {
    WindowProp property;

    std::string debug_path = (GetAppDirectory() / "Debug.txt").string();
    Log::Init(debug_path);
    SD_CORE_INFO("Debug info is output to: {}", debug_path);

    ini = CreateRef<Ini>();
    std::filesystem::path ini_path = GetAppDirectory() / SETTING_FILENAME;
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
    SetGraphicsAPI(api);

    ShaderLibrary::Instance().SetDirectory("assets");

    m_window = Window::Create(property);

    device = Device::Create(m_window.get());
    asset = CreateRef<AssetManager>();
    asset->SetLoader<Bitmap>(new BitmapLoader(*asset));
    asset->SetLoader<Model>(new ModelLoader(*asset));
    asset->SetLoader<Font>(new FontLoader(*asset));
    asset->Load("assets");

    renderer = CreateRef<Renderer>(device.get());

    dispatcher = CreateRef<EventDispatcher>();
    scene = CreateRef<Scene>();

    m_imgui = CreateLayer<ImGuiLayer>(m_window.get());
    PushOverlay(m_imgui);
}

Application::~Application() {
    asset->Save();

    glm::ivec2 size = m_window->GetSize();
    ini->SetInteger("window", "width", size.x);
    ini->SetInteger("window", "height", size.y);
    ini->SetInteger("window", "msaa", m_window->GetMSAA());
    ini->SetBoolean("window", "vsync", m_window->GetIsVSync());

    ini->Save((GetAppDirectory() / SETTING_FILENAME).string());

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

void Application::ProcessEvent(const Event &event) {
    if (event.type == EventType::APP_QUIT) {
        Quit();
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
    Event event;
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
    for (auto iter = m_layers.rbegin(); iter != m_layers.rend(); ++iter) {
        (*iter)->OnTick(dt);
    }
    Input::Tick();
}

void Application::Render() {
    for (auto &layer : m_layers) {
        layer->OnRender();
    }

    m_imgui->Begin();
    for (auto &layer : m_layers) {
        layer->OnImGui();
    }
    m_imgui->End();

    m_window->SwapBuffer();
}

}  // namespace SD
