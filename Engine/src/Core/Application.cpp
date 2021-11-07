#include "Core/Application.hpp"
#include "Utility/Timing.hpp"
#include "Utility/Log.hpp"
#include "Utility/Random.hpp"
#include "Utility/Loader/ShaderLoader.hpp"
#include "Utility/Loader/ImageLoader.hpp"
#include "Utility/Loader/ModelLoader.hpp"
#include "Utility/Loader/FontLoader.hpp"
#include "Graphics/Device.hpp"
#include "Renderer/RenderEngine.hpp"
#include "Input/InputEngine.hpp"

namespace sd {

Application *Application::s_instance = nullptr;

Application::Application() {
    s_instance = this;

    std::string debugPath = "Debug.txt";
    Log::init(debugPath);
    int width = 1280;
    int height = 720;
    int samples = 4;

    Random::init();
    SD_CORE_INFO("Debug info is output to: {}", debugPath);
    SDL(SDL_Init(SDL_INIT_EVERYTHING));

    Asset::manager().setRootPath("assets/");
    Asset::manager().setLoader<ShaderLoader>();
    Asset::manager().setLoader<ImageLoader>();
    Asset::manager().setLoader<ModelLoader>();
    Asset::manager().setLoader<FontLoader>();

    // Setting up which api to use
    Graphics::setAPI(API::OpenGL);

    // Initialize context
    WindowProp prop;
    prop.width = width;
    prop.height = height;
    prop.samples = samples;
    prop.flag = SDL_WINDOW_MAXIMIZED;
    Window::init(prop);

    // Intialize graphics device
    Device::init();
    if (samples > 1) {
        Device::instance().enable(Operation::MULTISAMPLE);
    } else {
        Device::instance().disable(Operation::MULTISAMPLE);
    }

    RenderEngine::init(width, height, samples);

    m_imguiLayer = std::static_pointer_cast<ImGuiLayer>(
        pushOverlay(createRef<ImGuiLayer>()));
}

Application::~Application() { SDL_Quit(); }

Ref<Layer> Application::pushLayer(const Ref<Layer> &layer) {
    return m_layers.pushLayer(layer);
}

Ref<Layer> Application::pushOverlay(const Ref<Layer> &layer) {
    return m_layers.pushOverlay(layer);
}

void Application::popLayer(const Ref<Layer> &layer) {
    m_layers.popLayer(layer);
}

void Application::popOverlay(const Ref<Layer> &layer) {
    m_layers.popOverlay(layer);
}

void Application::processEvent(const SDL_Event &event) {
    if (event.type == SDL_QUIT) {
        quit();
    }
    for (auto layer = m_layers.rbegin(); layer != m_layers.rend(); ++layer) {
        (*layer)->onEventProcess(event);
        if ((*layer)->isBlockEvent()) break;
    }
}

void Application::processEvents() {
    for (auto layer = m_layers.rbegin(); layer != m_layers.rend(); ++layer) {
        (*layer)->onEventsProcess();
        if ((*layer)->isBlockEvent()) break;
    }
}

void Application::run() {
    Clock clock;
    float minFps = 30;
    SDL_Event event;
    float msPerFrame = 1000.f / minFps;
    uint32_t msElapsed = 0;
    while (!Window::shouldClose()) {
        while (Window::pollEvent(event)) {
            InputEngine::processEvent(event);
            processEvent(event);
        }
        processEvents();

        msElapsed = clock.restart();
        while (msElapsed > msPerFrame) {
            msElapsed -= msPerFrame;
            tick(msPerFrame * 1e-3);
        }
        tick(msElapsed * 1e-3);

        render();
    }
}

void Application::quit() { Window::setShouldClose(true); }

void Application::tick(float dt) {
    InputEngine::tick();
    RenderEngine::tick(dt);

    for (auto iter = m_layers.rbegin(); iter != m_layers.rend(); ++iter) {
        (*iter)->onTick(dt);
    }
}

void Application::render() {
    RenderEngine::render();

    for (auto &layer : m_layers) {
        layer->onRender();
    }
    RenderEngine::postRender();

    m_imguiLayer->begin();
    for (auto &layer : m_layers) {
        layer->onImGui();
    }
    m_imguiLayer->end();

    Window::swapBuffer();
}

}  // namespace sd
