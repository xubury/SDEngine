#ifndef SD_ENGINE_HPP
#define SD_ENGINE_HPP

#include "Core/Window.hpp"
#include "Core/LayerStack.hpp"
#include "Core/RenderEngine.hpp"
#include "Core/InputEngine.hpp"
#include "ImGui/ImGuiLayer.hpp"

int main(int argc, char **argv);

namespace sd {

class SD_API Application {
   public:
    static Window &getWindow();
    static RenderEngine &getRenderEngine();
    static InputEngine &getInputEngine();
    static void quit();

    Application(const Application &application) = delete;
    Application &operator=(const Application &application) = delete;

   protected:
    void pushLayer(Layer *layer);

    void pushOverlay(Layer *layer);

    void popLayer(Layer *layer);

    void popOverlay(Layer *layer);

    Application();
    ~Application();

   private:
    friend int ::main(int argc, char **argv);

    virtual void init() = 0;

    void run();

    virtual void destroy() = 0;

    static Application *s_instance;

    void processEvent(const SDL_Event &event);

    void processEvents();

    void tick(float dt);
    void render();

    Window m_window;
    LayerStack m_layers;
    ImGuiLayer *m_imguiLayer;
    RenderEngine *m_renderEngine;
    InputEngine *m_inputEngine;
};

}  // namespace sd

#endif /* SD_ENGINE_HPP */
