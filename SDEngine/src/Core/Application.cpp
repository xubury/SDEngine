

#include "Core/Application.hpp"
#include "Core/SDL.hpp"
#include "Core/Input.hpp"
#include "Utility/Timing.hpp"
#include "Utility/Random.hpp"

#include "Asset/AssetStorage.hpp"
#include "Asset/ModelAsset.hpp"
#include "Asset/FontAsset.hpp"
#include "Asset/TextureAsset.hpp"
#include "Asset/SceneAsset.hpp"

#if defined(SD_PLATFORM_LINUX)
#include <unistd.h>
#endif

#if defined(SD_PLATFORM_WINDOWS)
#include <libloaderapi.h>
#endif

namespace SD {

std::filesystem::path Application::GetAppDirectory()
{
    static std::filesystem::path path;
    if (path.empty()) {
#if defined(SD_PLATFORM_LINUX)
        char buffer[260];
        size_t size = readlink("/proc/self/exe", buffer, sizeof(buffer));
        if (size > 0) {
            path.assign(std::begin(buffer), std::begin(buffer) + size);
        }
#elif defined(SD_PLATFORM_WINDOWS)
        char buffer[255];
        size_t size = GetModuleFileNameA(NULL, buffer, sizeof(path));
        if (size > 0) {
            path.assign(std::begin(buffer), std::begin(buffer) + size);
        }
#endif
    }
    return path.parent_path();
}

const std::string SETTING_FILENAME = "setting.ini";

Application *Application::s_instance;

static void RegisterAssets(AssetStorage *storage)
{
    storage->RegisterAsset<FontAsset>(
        AssetTypeData{0, std::bind(Asset::Create<FontAsset>),
                      std::bind(Asset::Destroy, std::placeholders::_1)});
    storage->RegisterAsset<TextureAsset>(
        AssetTypeData{0, std::bind(Asset::Create<TextureAsset>),
                      std::bind(Asset::Destroy, std::placeholders::_1)});
    storage->RegisterAsset<ModelAsset>(
        AssetTypeData{1, std::bind(Asset::Create<ModelAsset>),
                      std::bind(Asset::Destroy, std::placeholders::_1)});
    storage->RegisterAsset<SceneAsset>(
        AssetTypeData{2, std::bind(Asset::Create<SceneAsset>),
                      std::bind(Asset::Destroy, std::placeholders::_1)});
}

Application::Application(const std::string &title, Device::API api)
{
    std::string debug_path = (GetAppDirectory() / "Debug.txt").generic_string();
    Log::Init(debug_path);
    SD_CORE_INFO("Debug info is output to: {}", debug_path);

    // Setting up which api to use
    Device::SetAPI(api);

    InitSettings();

    SDL(SDL_Init(SDL_INIT_EVERYTHING));

    m_dispatcher = CreateRef<EventDispatcher>();

    Input::Init(m_dispatcher.get());
    Input::Init(m_dispatcher.get());

    WindowCreateInfo property{
        title,
        m_settings.GetInteger("window", "x", SDL_WINDOWPOS_CENTERED),
        m_settings.GetInteger("window", "y", SDL_WINDOWPOS_CENTERED),
        m_settings.GetInteger("window", "width", 800),
        m_settings.GetInteger("window", "height", 600),
        static_cast<MultiSampleLevel>(
            m_settings.GetInteger("window", "msaa", 4)),
        m_settings.GetBoolean("window", "vsync", true),
        SDL_WINDOW_RESIZABLE};
    m_window = Window::Create(property);
    m_window->SetDispatcher(m_dispatcher.get());

    AssetStorage::Init();

    auto &storage = AssetStorage::Get();
    RegisterAssets(&storage);
    storage.SetDirectory("assets");
    storage.ScanDirectory(storage.GetDirectory());
}

Application::~Application()
{
    AssetStorage::Shutdown();

    glm::ivec2 size = m_window->GetSize();
    m_settings.SetInteger("window", "width", size.x);
    m_settings.SetInteger("window", "height", size.y);
    m_settings.SetInteger("window", "msaa",
                          static_cast<int>(m_window->GetMSAA()));
    m_settings.SetBoolean("window", "vsync", m_window->GetIsVSync());

    m_settings.Save((GetAppDirectory() / SETTING_FILENAME).string());

    SDL_Quit();
}

void Application::OnInit()
{
    m_quit_handler = m_dispatcher->Register<AppQuitEvent>(
        [this](const AppQuitEvent &) { Shutdown(); });

    m_imgui = CreateLayer<ImGuiLayer>(m_window.get());
    PushOverlay(m_imgui);
}

void Application::OnDestroy()
{
    m_dispatcher->RemoveHandler(m_quit_handler);
    while (m_layers.Size()) {
        auto layer = m_layers.Front();
        PopLayer(layer);
        DestroyLayer(layer);
    }
}

void Application::PushLayer(Layer *layer)
{
    layer->OnPush();
    m_layers.Push(layer);
}

void Application::PushOverlay(Layer *layer)
{
    layer->OnPush();
    m_layers.PushOverlay(layer);
}

void Application::PopLayer(Layer *layer)
{
    layer->OnPop();
    m_layers.Pop(layer);
}

void Application::DestroyLayer(Layer *layer)
{
    layer->OnDestroy();
    delete layer;
}

void Application::InitSettings()
{
    std::filesystem::path ini_path = GetAppDirectory() / SETTING_FILENAME;
    if (std::filesystem::exists(ini_path)) {
        m_settings.Load(ini_path.string());
    }
    else {
        SD_CORE_WARN(
            "No such ini file: {}. The application will create a new one.",
            ini_path);
    }
}

void Application::Run()
{
    Clock clock;
    float min_fps = 30;
    float ms_per_frame = 1000.f / min_fps;
    uint32_t ms_elapsed = 0;
    while (!m_window->ShouldClose()) {
        m_window->PollEvents();

        ms_elapsed = clock.Restart();
        while (ms_elapsed > ms_per_frame) {
            ms_elapsed -= ms_per_frame;
            Tick(ms_per_frame * 1e-3);
        }
        Tick(ms_elapsed * 1e-3);

        Render();
    }
}

void Application::Shutdown() { m_window->SetShouldClose(true); }

void Application::Tick(float dt)
{
    for (auto iter = m_layers.rbegin(); iter != m_layers.rend(); ++iter) {
        (*iter)->OnTick(dt);
    }
    Input::Tick();
}

void Application::Render()
{
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
