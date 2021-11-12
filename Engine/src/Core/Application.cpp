#include "Core/Application.hpp"
#include "Utility/Timing.hpp"
#include "Utility/Random.hpp"
#include "Renderer/Renderer.hpp"
#include "Input/Input.hpp"

namespace SD {

static Application *s_instance = nullptr;

Application &getApp() { return *s_instance; }

Window &Application::getWindow() { return *m_window; }

Application::Application() {
    s_instance = this;

    std::string debugPath = "Debug.txt";
    Log::init(debugPath);
    int width = 1600;
    int height = 900;
    int samples = 4;

    Random::init();
    SD_CORE_INFO("Debug info is output to: {}", debugPath);
    SDL(SDL_Init(SDL_INIT_EVERYTHING));

    // Setting up which api to use
    setGraphicsAPI(GraphicsAPI::OpenGL);

    // Initialize context
    WindowProp prop;
    prop.width = width;
    prop.height = height;
    prop.samples = samples;
    prop.flag = SDL_WINDOW_MAXIMIZED;
    m_window = Window::create(prop);

    // Intialize graphics device
    ShaderLibrary::instance().setRootPath("assets");
    Renderer::engine().init(width, height, samples);
    if (samples > 1) {
        Renderer::device().enable(Operation::MULTISAMPLE);
    } else {
        Renderer::device().disable(Operation::MULTISAMPLE);
    }

    pushLayer(&Renderer::engine());
    m_imguiLayer = dynamic_cast<ImGuiLayer *>(pushOverlay(new ImGuiLayer()));
}

Application::~Application() {
    delete m_imguiLayer;
    SDL_Quit();
}

Layer *Application::pushLayer(Layer *layer) {
    return m_layers.pushLayer(layer);
}

Layer *Application::pushOverlay(Layer *layer) {
    return m_layers.pushOverlay(layer);
}

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
    while (!m_window->shouldClose()) {
        while (m_window->pollEvent(event)) {
            Input::processEvent(event);
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

void Application::quit() { m_window->setShouldClose(true); }

void Application::tick(float dt) {
    Input::tick();

    for (auto iter = m_layers.rbegin(); iter != m_layers.rend(); ++iter) {
        (*iter)->onTick(dt);
    }
}

void Application::render() {
    for (auto &layer : m_layers) {
        layer->onRender();
    }

    m_imguiLayer->begin();
    for (auto &layer : m_layers) {
        layer->onImGui();
    }
    m_imguiLayer->end();

    m_window->swapBuffer();
}

}  // namespace SD
