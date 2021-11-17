#include "Core/Application.hpp"
#include "Core/SDL.hpp"
#include "Utility/Timing.hpp"
#include "Utility/Random.hpp"
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
    int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF;
    SD_CORE_ASSERT((IMG_Init(imgFlags) & imgFlags) == imgFlags, IMG_GetError());

    // Setting up which api to use
    setGraphicsAPI(GraphicsAPI::OpenGL);

    WindowProp prop;
    prop.width = width;
    prop.height = height;
    prop.samples = samples;
    prop.flag = SDL_WINDOW_MAXIMIZED;
    m_window = Window::create(prop);

    ShaderLibrary::instance().setRootPath("assets");

    renderer = createRef<Renderer>(samples);
    asset = createRef<AssetManager>("assets");
    dispatcher = createRef<EventDispatcher>();

    m_imguiLayer = new ImGuiLayer();
    pushOverlay(m_imguiLayer);
}

Application::~Application() {
    while (m_layers.size()) {
        destroyLayer(m_layers.front());
    }
    IMG_Quit();
    SDL_Quit();
}

void Application::pushLayer(Layer *layer) {
    layer->setAppVars(makeAppVars());
    layer->onPush();
    m_layers.push(layer);
}

void Application::pushOverlay(Layer *layer) {
    layer->setAppVars(makeAppVars());
    layer->onPush();
    m_layers.pushOverlay(layer);
}

void Application::popLayer(Layer *layer) {
    layer->onPop();
    m_layers.pop(layer);
}

void Application::destroyLayer(Layer *layer) {
    popLayer(layer);
    delete layer;
}

void Application::processEvent(const SDL_Event &sdl_event) {
    if (sdl_event.type == SDL_QUIT) {
        quit();
    }
    Event event;
    switch (sdl_event.type) {
        case SDL_MOUSEMOTION:
            event.type = Event::EventType::MOUSE_MOTION;
            event.mouseMotion.x = sdl_event.motion.x;
            event.mouseMotion.y = sdl_event.motion.y;
            event.mouseMotion.xrel = sdl_event.motion.xrel;
            event.mouseMotion.yrel = sdl_event.motion.yrel;
            Input::setMouseCoord(event.mouseMotion.x, event.mouseMotion.y);
            break;
        case SDL_MOUSEBUTTONDOWN:
            event.type = Event::EventType::MOUSE_BUTTON_PRESSED;
            event.mouseButton.button =
                static_cast<MouseButton>(sdl_event.button.button);
            SDL_BUTTON_LEFT;
            event.mouseButton.x = sdl_event.button.x;
            event.mouseButton.y = sdl_event.button.y;
            event.mouseButton.clicks = sdl_event.button.clicks;
            Input::pressMouseButton(event.mouseButton.button);
            break;
        case SDL_MOUSEBUTTONUP:
            event.type = Event::EventType::MOUSE_BUTTON_RELEASED;
            event.mouseButton.button =
                static_cast<MouseButton>(sdl_event.button.button);
            event.mouseButton.x = sdl_event.button.x;
            event.mouseButton.y = sdl_event.button.y;
            event.mouseButton.clicks = sdl_event.button.clicks;
            Input::releaseMouseButton(event.mouseButton.button);
            break;
        case SDL_MOUSEWHEEL:
            event.type = Event::EventType::MOUSE_WHEEL_SCROLLED;
            event.mouseWheel.x = sdl_event.wheel.x;
            event.mouseWheel.y = sdl_event.wheel.y;
            break;
        case SDL_KEYDOWN:
            event.type = Event::EventType::KEY_PRESSED;
            event.key.keycode = static_cast<Keycode>(sdl_event.key.keysym.sym);
            event.key.mod = sdl_event.key.keysym.mod;
            Input::pressKey(event.key.keycode);
            break;
        case SDL_KEYUP:
            event.type = Event::EventType::KEY_RELEASED;
            event.key.keycode = static_cast<Keycode>(sdl_event.key.keysym.sym);
            event.key.mod = sdl_event.key.keysym.mod;
            Input::releaseKey(event.key.keycode);
            break;
        case SDL_WINDOWEVENT:
            switch (sdl_event.window.type) {
                case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_SIZE_CHANGED: {
                    event.type = Event::EventType::WINDOW_RESIZED;
                    event.size.width = sdl_event.window.data1;
                    event.size.height = sdl_event.window.data2;
                } break;
            }
            break;
        case SDL_TEXTINPUT: {
            event.type = Event::EventType::TEXT_INPUT;
            std::copy(std::begin(sdl_event.text.text),
                      std::end(sdl_event.text.text), event.text.text);
        } break;
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
