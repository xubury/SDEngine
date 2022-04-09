#include "Core/Application.hpp"
#include "Core/InputLayer.hpp"
#include "Core/ScriptLayer.hpp"
#include "Utility/Timing.hpp"
#include "Utility/Random.hpp"

#include "Asset/AssetStorage.hpp"
#include "Asset/ModelAsset.hpp"
#include "Asset/FontAsset.hpp"
#include "Asset/TextureAsset.hpp"
#include "Asset/SceneAsset.hpp"
#include "Asset/ScriptAsset.hpp"

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
        char buffer[255];
#if defined(SD_PLATFORM_LINUX)
        size_t size = readlink("/proc/self/exe", buffer, sizeof(buffer));
#elif defined(SD_PLATFORM_WINDOWS)
        size_t size = GetModuleFileNameA(NULL, buffer, sizeof(path));
#endif
        if (size > 0) {
            path.assign(std::begin(buffer), std::begin(buffer) + size);
        }
    }
    return path.parent_path();
}

const std::string SETTING_FILENAME = "setting.ini";

Application *Application::s_instance;

static void RegisterAssets(AssetStorage *storage)
{
    storage->RegisterAsset<FontAsset>(AssetTypeData{
        0, std::bind(Asset::Create<FontAsset>),
        std::bind(Asset::Destroy, std::placeholders::_1), ".sdfont"});
    storage->RegisterAsset<TextureAsset>(AssetTypeData{
        0, std::bind(Asset::Create<TextureAsset>),
        std::bind(Asset::Destroy, std::placeholders::_1), ".sdtexture"});
    storage->RegisterAsset<ModelAsset>(AssetTypeData{
        1, std::bind(Asset::Create<ModelAsset>),
        std::bind(Asset::Destroy, std::placeholders::_1), ".sdmodel"});
    storage->RegisterAsset<SceneAsset>(AssetTypeData{
        2, std::bind(Asset::Create<SceneAsset>),
        std::bind(Asset::Destroy, std::placeholders::_1), ".sdscene"});
    storage->RegisterAsset<ScriptAsset>(AssetTypeData{
        0, std::bind(Asset::Create<ScriptAsset>),
        std::bind(Asset::Destroy, std::placeholders::_1), ".sdscript"});
}

Application::Application(const std::string &title, Device::API api)
    : m_imgui_layer(nullptr)
{
    const std::string debug_path =
        (GetAppDirectory() / "Debug.txt").generic_string();
    Log::Init(debug_path);
    SD_CORE_INFO("Debug info is output to: {}", debug_path);

    // Setting up which api to use
    Device::SetAPI(api);

    LoadSettingsFile();

    WindowCreateInfo property{title,
                              m_settings.GetInteger("window", "width", 800),
                              m_settings.GetInteger("window", "height", 600),
                              static_cast<MultiSampleLevel>(
                                  m_settings.GetInteger("window", "msaa", 4)),
                              m_settings.GetBoolean("window", "vsync", true)};
    m_window = Window::Create(property);
    m_device = Device::Create();

    AssetStorage::Init();
    auto &storage = AssetStorage::Get();
    RegisterAssets(&storage);
    storage.SetDirectory("assets");
    // storage.ScanDirectory(storage.GetDirectory());
}

Application::~Application() {}

void Application::OnInit()
{
    m_graphics_layer =
        CreateLayer<GraphicsLayer>(m_device.get(), m_window->GetWidth(),
                                   m_window->GetHeight(), m_window->GetMSAA());
    PushLayer(m_graphics_layer);
    PushLayer(CreateLayer<ScriptLayer>());
    PushLayer(CreateLayer<InputLayer>());
}

void Application::OnDestroy()
{
    while (m_layers.Size()) {
        auto layer = m_layers.Front();
        PopLayer(layer);
        DestroyLayer(layer);
    }
    AssetStorage::Shutdown();
    Vector2i size = m_window->GetSize();
    m_settings.SetInteger("window", "width", size.x);
    m_settings.SetInteger("window", "height", size.y);
    m_settings.SetInteger("window", "msaa",
                          static_cast<int>(m_window->GetMSAA()));
    m_settings.SetBoolean("window", "vsync", m_window->GetIsVSync());

    m_settings.Save((GetAppDirectory() / SETTING_FILENAME).string());
}

void Application::PushLayer(Layer *layer)
{
    if (m_layers.Has(layer)) {
        SD_CORE_WARN("Trying to push layer that already in the stack!");
        return;
    }
    layer->OnPush();
    m_layers.Push(layer);
}

void Application::PushOverlay(Layer *layer)
{
    if (m_layers.Has(layer)) {
        SD_CORE_WARN("Trying to push layer that already in the stack!");
        return;
    }
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

void Application::CreateImGuiLayer()
{
    m_imgui_layer = CreateLayer<ImGuiLayer>(m_window.get());
    PushOverlay(m_imgui_layer);
#ifdef SD_IMGUI_IMPORT
    // for DLL context
    ImGui::SetCurrentContext(m_imgui_layer->GetContext());
#endif
}

void Application::LoadSettingsFile()
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
    const float min_fps = 30;
    const float ms_per_frame = 1000.f / min_fps;
    uint32_t ms_elapsed = 0;
    while (!m_window->ShouldClose()) {
        m_window->PollEvents(m_layers);

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
}

void Application::Render()
{
    for (auto &layer : m_layers) {
        layer->OnRender();
    }

    if (m_imgui_layer) {
        m_imgui_layer->Begin();
        for (auto &layer : m_layers) {
            layer->OnImGui();
        }
        m_imgui_layer->End();
    }

    m_window->SwapBuffer();
}

}  // namespace SD
