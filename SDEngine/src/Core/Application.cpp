#include "Core/Application.hpp"
#include "Core/SDL.hpp"
#include "Core/Input.hpp"
#include "Renderer/Renderer.hpp"
#include "Utility/Timing.hpp"
#include "Utility/Random.hpp"

#include "Asset/AssetStorage.hpp"
#include "Asset/ModelAsset.hpp"
#include "Asset/FontAsset.hpp"
#include "Asset/TextureAsset.hpp"
#include "Asset/SceneAsset.hpp"

namespace SD {

const std::string SETTING_FILENAME = "setting.ini";

Application::Application(const std::string &title, GraphicsAPI api) {
    std::string debug_path = (GetAppDirectory() / "Debug.txt").generic_string();
    Log::Init(debug_path);
    SD_CORE_INFO("Debug info is output to: {}", debug_path);

    // Setting up which api to use
    SetGraphicsAPI(api);

    InitSettings();

    SDL(SDL_Init(SDL_INIT_EVERYTHING));

    WindowProp property(
        title, setting->GetInteger("window", "x", SDL_WINDOWPOS_CENTERED),
        setting->GetInteger("window", "y", SDL_WINDOWPOS_CENTERED),
        setting->GetInteger("window", "width", 800),
        setting->GetInteger("window", "height", 600),
        setting->GetInteger("window", "msaa", 4),
        setting->GetBoolean("window", "vsync", true), SDL_WINDOW_RESIZABLE);
    m_window = Window::Create(property);

    EventSystem::Init();
    AssetStorage::Init();
    Device::Init(m_window.get());
    Renderer::Init();

    AssetStorage::Get().SetDirectory("assets");
    RegisterAssets();

    scene = CreateRef<Scene>();
}

Application::~Application() {
    Renderer::Shutdown();
    Device::Shutdown();
    AssetStorage::Shutdown();
    EventSystem::Shutdown();

    glm::ivec2 size = m_window->GetSize();
    setting->SetInteger("window", "width", size.x);
    setting->SetInteger("window", "height", size.y);
    setting->SetInteger("window", "msaa", m_window->GetMSAA());
    setting->SetBoolean("window", "vsync", m_window->GetIsVSync());

    setting->Save((GetAppDirectory() / SETTING_FILENAME).string());

    SDL_Quit();
}

void Application::OnInit() {
    m_imgui = CreateLayer<ImGuiLayer>(m_window.get());
    PushOverlay(m_imgui);
}

void Application::OnDestroy() {
    while (m_layers.Size()) {
        auto layer = m_layers.Front();
        PopLayer(layer);
        DestroyLayer(layer);
    }
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
    layer->OnDestroy();
    delete layer;
}

void Application::InitSettings() {
    setting = CreateRef<Ini>();
    std::filesystem::path ini_path = GetAppDirectory() / SETTING_FILENAME;
    if (std::filesystem::exists(ini_path)) {
        setting->Load(ini_path.string());
    } else {
        SD_CORE_WARN(
            "No such ini file: {}. The application will create a new one.",
            ini_path);
    }
}

void Application::RegisterAssets() {
    AssetStorage::Get().RegisterAsset<FontAsset>(AssetTypeData{
        0, std::bind(Asset::Create<FontAsset>),
        std::bind(Asset::Destroy<FontAsset>, std::placeholders::_1)});
    AssetStorage::Get().RegisterAsset<TextureAsset>(AssetTypeData{
        0, std::bind(Asset::Create<TextureAsset>),
        std::bind(Asset::Destroy<TextureAsset>, std::placeholders::_1)});
    AssetStorage::Get().RegisterAsset<ModelAsset>(AssetTypeData{
        1, std::bind(Asset::Create<ModelAsset>),
        std::bind(Asset::Destroy<ModelAsset>, std::placeholders::_1)});
    AssetStorage::Get().RegisterAsset<SceneAsset>(AssetTypeData{
        2, std::bind(Asset::Create<SceneAsset>),
        std::bind(Asset::Destroy<SceneAsset>, std::placeholders::_1)});
}

void Application::ProcessEvent(const ApplicationEvent &event) {
    if (event.type == EventType::APP_QUIT) {
        Quit();
    }
    for (auto layer = m_layers.rbegin(); layer != m_layers.rend(); ++layer) {
        (*layer)->OnEventProcess(event);
        if ((*layer)->IsBlockEvent()) break;
    }
}

void Application::Run() {
    Clock clock;
    float min_fps = 30;
    ApplicationEvent event;
    float ms_per_frame = 1000.f / min_fps;
    uint32_t ms_elapsed = 0;
    while (!m_window->ShouldClose()) {
        while (m_window->PollEvent(event)) {
            ProcessEvent(event);
        }

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
