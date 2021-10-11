#include "Core/Application.hpp"
#include "Core/Input.hpp"
#include "Core/Timing.hpp"
#include "Utility/Log.hpp"
#include "Utility/Random.hpp"
#include "Utility/Loader/ShaderLoader.hpp"
#include "Utility/Loader/TextureLoader.hpp"
#include "Utility/Loader/ModelLoader.hpp"
#include "Utility/Loader/FontLoader.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Renderer.hpp"

namespace sd {

Application *Application::s_instance = nullptr;

Window &Application::getWindow() { return s_instance->m_window; }

RenderEngine &Application::getRenderEngine() {
    return *s_instance->m_renderEngine;
}

InputEngine &Application::getInputEngine() {
    return *s_instance->m_inputEngine;
}

Application::Application() {
    s_instance = this;

    std::string debugPath = "Debug.txt";
    Log::init(debugPath);
    int width = 1280;
    int height = 720;
    int samples = 8;

    Random::init();
    SD_CORE_INFO("Debug info is output to: {}", debugPath);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        SD_CORE_ERROR("SDL_Init failed: {}", SDL_GetError());
        exit(-1);
    }

    Asset::manager().setRootPath("assets/");
    Asset::manager().setLoader<ShaderLoader>();
    Asset::manager().setLoader<TextureLoader>();
    Asset::manager().setLoader<ModelLoader>();
    Asset::manager().setLoader<FontLoader>();

    Graphics::init(API::OpenGL);

    WindowProp prop;
    prop.width = width;
    prop.height = height;
    prop.samples = samples;
    if (!m_window.create(prop)) {
        exit(-1);
    }

    Device::create();
    Renderer::init();

    m_inputEngine = new InputEngine();
    m_imguiLayer = new ImGuiLayer();
    m_renderEngine = new RenderEngine(width, height, samples);
    pushOverlay(m_imguiLayer);
    pushOverlay(m_inputEngine);
    pushLayer(m_renderEngine);
}

Application::~Application() { SDL_Quit(); }

void Application::pushLayer(Layer *layer) { m_layers.pushLayer(layer); }

void Application::pushOverlay(Layer *layer) { m_layers.pushOverlay(layer); }

void Application::popLayer(Layer *layer) { m_layers.popLayer(layer); }

void Application::popOverlay(Layer *layer) { m_layers.popOverlay(layer); }

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
    while (!m_window.shouldClose()) {
        while (m_window.pollEvent(event)) {
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

void Application::quit() { s_instance->m_window.setShouldClose(true); }

void Application::tick(float dt) {
    for (auto iter = m_layers.rbegin(); iter != m_layers.rend(); ++iter) {
        auto &layer = *iter;
        for (auto &system : layer->getSystems()) {
            system->onTick(dt);
        }
        layer->onTick(dt);
    }
}

void Application::render() {
    for (auto &layer : m_layers) {
        for (auto &system : layer->getSystems()) {
            system->onRender();
        }
        layer->onRender();
    }
    m_imguiLayer->begin();
    for (auto &layer : m_layers) {
        layer->onImGui();
    }
    m_imguiLayer->end();

    m_window.swapBuffer();
}

}  // namespace sd
