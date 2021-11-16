#include "Core/Application.hpp"
#include "Utility/Timing.hpp"
#include "Utility/Random.hpp"
#include "Input/Input.hpp"

#include <SDL.h>

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

void Application::processEvent(const SDL_Event &sdlEvent) {
    if (sdlEvent.type == SDL_QUIT) {
        quit();
    }
    Event event;
    switch (sdlEvent.type) {
        case SDL_MOUSEMOTION:
            event.type = Event::EventType::MOUSE_MOTION;
            event.mouseMotion.x = sdlEvent.motion.x;
            event.mouseMotion.y = sdlEvent.motion.y;
            event.mouseMotion.xrel = sdlEvent.motion.xrel;
            event.mouseMotion.yrel = sdlEvent.motion.yrel;
            Input::setMouseCoord(event.mouseMotion.x, event.mouseMotion.y);
            break;
        case SDL_MOUSEBUTTONDOWN:
            event.type = Event::EventType::MOUSE_BUTTON_PRESSED;
            event.mouseButton.button =
                static_cast<MouseButton>(sdlEvent.button.button);
            SDL_BUTTON_LEFT;
            event.mouseButton.x = sdlEvent.button.x;
            event.mouseButton.y = sdlEvent.button.y;
            event.mouseButton.clicks = sdlEvent.button.clicks;
            Input::pressMouseButton(event.mouseButton.button);
            break;
        case SDL_MOUSEBUTTONUP:
            event.type = Event::EventType::MOUSE_BUTTON_RELEASED;
            event.mouseButton.button =
                static_cast<MouseButton>(sdlEvent.button.button);
            event.mouseButton.x = sdlEvent.button.x;
            event.mouseButton.y = sdlEvent.button.y;
            event.mouseButton.clicks = sdlEvent.button.clicks;
            Input::releaseMouseButton(event.mouseButton.button);
            break;
        case SDL_MOUSEWHEEL:
            event.type = Event::EventType::MOUSE_WHEEL_SCROLLED;
            event.mouseWheel.x = sdlEvent.wheel.x;
            event.mouseWheel.y = sdlEvent.wheel.y;
            break;
        case SDL_KEYDOWN:
            event.type = Event::EventType::KEY_PRESSED;
            event.key.keycode = static_cast<Keycode>(sdlEvent.key.keysym.sym);
            event.key.scancode = sdlEvent.key.keysym.scancode;
            event.key.mod = sdlEvent.key.keysym.mod;
            Input::pressKey(event.key.keycode);
            break;
        case SDL_KEYUP:
            event.type = Event::EventType::KEY_RELEASED;
            event.key.keycode = static_cast<Keycode>(sdlEvent.key.keysym.sym);
            event.key.scancode = sdlEvent.key.keysym.scancode;
            event.key.mod = sdlEvent.key.keysym.mod;
            Input::releaseKey(event.key.keycode);
            break;
        case SDL_WINDOWEVENT:
            switch (sdlEvent.window.type) {
                case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    event.type = Event::EventType::WINDOW_RESIZED;
                    break;
            }
            break;
        case SDL_TEXTINPUT: {
            event.type = Event::EventType::TEXT_INPUT;
            std::copy(std::begin(sdlEvent.text.text),
                      std::end(sdlEvent.text.text), event.text.text);
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
