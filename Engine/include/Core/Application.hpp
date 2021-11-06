#ifndef SD_ENGINE_HPP
#define SD_ENGINE_HPP

#include "Core/Window.hpp"
#include "Core/LayerStack.hpp"
#include "ImGui/ImGuiLayer.hpp"

int main(int argc, char **argv);

namespace sd {

class SD_API Application {
   public:
    static void quit();

    Application(const Application &application) = delete;
    Application &operator=(const Application &application) = delete;

   protected:
    Ref<Layer> pushLayer(const Ref<Layer> &layer);

    Ref<Layer> pushOverlay(const Ref<Layer> &layer);

    void popLayer(const Ref<Layer> &layer);

    void popOverlay(const Ref<Layer> &layer);

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

    LayerStack m_layers;
    Ref<ImGuiLayer> m_imguiLayer;
};

}  // namespace sd

#endif /* SD_ENGINE_HPP */
