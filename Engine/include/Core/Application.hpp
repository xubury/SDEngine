#ifndef SD_ENGINE_HPP
#define SD_ENGINE_HPP

#include "Core/Window.hpp"
#include "Core/LayerStack.hpp"
#include "Utils/AssetManager.hpp"
#include "ImGui/ImGuiLayer.hpp"
#include "Renderer/RenderEngine.hpp"

namespace sd {

class SD_API Application {
   public:
    static Application &instance();

    static Window &getWindow();
    static RenderEngine &getRenderEngine();

    Application(const Application &application) = delete;
    Application &operator=(const Application &application) = delete;

    virtual void init() = 0;

    virtual void destroy() = 0;

    void pushLayer(Layer *layer);

    void pushOverlay(Layer *layer);

    void popLayer(Layer *layer);

    void popOverlay(Layer *layer);

    void run();

    void quit();

   protected:
    Application();
    ~Application();

   private:
    static Application *s_instance;

    void processEvent(const SDL_Event &event);

    void processEvents();

    void tick(float dt);
    void render();

    Window m_window;
    LayerStack m_layers;
    ImGuiLayer *m_imguiLayer;
    SystemManager m_systems;

    RenderEngine m_renderEngine;
};

}  // namespace sd

#endif /* SD_ENGINE_HPP */
