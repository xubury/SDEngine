#ifndef SD_ENGINE_HPP
#define SD_ENGINE_HPP

#include "Core/Vars.hpp"
#include "Core/Window.hpp"
#include "Core/LayerStack.hpp"
#include "ImGui/ImGuiLayer.hpp"

int main(int argc, char **argv);

namespace SD {

class SD_API Application {
   public:
    void quit();

    Application(const Application &application) = delete;
    Application &operator=(const Application &application) = delete;

    Window &getWindow();

   protected:
    void pushLayer(Layer *layer);

    void pushOverlay(Layer *layer);

    void popLayer(Layer *layer);

    void destroyLayer(Layer *layer);

    Application();
    virtual ~Application();

    APP_VARS
   private:
    SET_APP_VARS
    MAKE_APP_VARS

    friend int ::main(int argc, char **argv);

    virtual void onInit() = 0;

    void run();

    void processEvent(const SDL_Event &event);

    void processEvents();

    void tick(float dt);
    void render();

    Ref<Window> m_window;

    LayerStack m_layers;
    ImGuiLayer *m_imguiLayer;
};

Application &getApp();

}  // namespace SD

#endif /* SD_ENGINE_HPP */
