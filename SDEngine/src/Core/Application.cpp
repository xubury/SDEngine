#include "Core/Application.hpp"
#include "Core/InputLayer.hpp"
#include "Core/ScriptLayer.hpp"
#include "Utility/Timing.hpp"
#include "Utility/Random.hpp"

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

const std::string setting_filename = "Setting.ini";
const std::string debug_filename = "Debug.txt";

Application *Application::s_instance;

Application::Application(const std::string &title, Device::API api)
    : m_imgui_layer(nullptr)
{
    const std::string debug_path =
        (GetAppDirectory() / debug_filename).generic_string();
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

    // TODO: Loading default assets.
    // Should move this to a asset table file, so we can load asset dynamically
    {
        TextureParameter icon_params{
            TextureWrap::Repeat, TextureMinFilter::Linear,
            TextureMagFilter::Linear, MipmapMode::Linear};
        m_resources.textures.Load("icon/light", "assets/icons/light.png",
                                  icon_params);
        m_resources.textures.Load("icon/file", "assets/icons/FileIcon.png",
                                  icon_params);
        m_resources.textures.Load(
            "icon/directory", "assets/icons/DirectoryIcon.png", icon_params);
        m_resources.textures.Load("skybox/default",
                                  std::array<std::string_view, 6>{
                                      "assets/skybox/right.jpg",
                                      "assets/skybox/left.jpg",
                                      "assets/skybox/top.jpg",
                                      "assets/skybox/bottom.jpg",
                                      "assets/skybox/front.jpg",
                                      "assets/skybox/back.jpg",
                                  });
    }
}

Application::~Application() {}

void Application::OnInit()
{
    m_graphics_layer = CreateLayer<GraphicsLayer>(
        &m_resources, &m_scenes, m_device.get(), m_window->GetWidth(),
        m_window->GetHeight(), m_window->GetMSAA());
    PushLayer(m_graphics_layer);
    PushLayer(CreateLayer<ScriptLayer>());
    PushLayer(CreateLayer<InputLayer>(&m_input));
}

void Application::OnDestroy()
{
    while (m_layers.Size()) {
        auto layer = m_layers.Front();
        PopLayer(layer);
        DestroyLayer(layer);
    }
    Vector2i size = m_window->GetSize();
    m_settings.SetInteger("window", "width", size.x);
    m_settings.SetInteger("window", "height", size.y);
    m_settings.SetInteger("window", "msaa",
                          static_cast<int>(m_window->GetMSAA()));
    m_settings.SetBoolean("window", "vsync", m_window->GetIsVSync());

    m_settings.Save((GetAppDirectory() / setting_filename).string());
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
    std::filesystem::path ini_path = GetAppDirectory() / setting_filename;
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
