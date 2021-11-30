#ifndef SD_ENGINE_HPP
#define SD_ENGINE_HPP

#include "Core/Vars.hpp"
#include "Core/Window.hpp"
#include "ImGui/ImGuiLayer.hpp"

int main(int argc, char **argv);

namespace SD {

class SD_API Application {
   public:
    void Quit();

   protected:
    void PushLayer(Layer *layer);

    void PushOverlay(Layer *layer);

    void PopLayer(Layer *layer);

    void DestroyLayer(Layer *layer);

    Application(const WindowProp &property);

    virtual ~Application() = default;
    Application(const Application &application) = delete;
    Application &operator=(const Application &application) = delete;

    APP_VARS
   private:
    SET_APP_VARS
    MAKE_APP_VARS

    friend int ::main(int argc, char **argv);

    void OnInit();
    void OnDestroy();
    virtual void OnStart() = 0;
    virtual void OnExit() = 0;

    void Run();

    void ProcessEvent(const SDL_Event &event);

    void ProcessEvents();

    void Tick(float dt);
    void Render();

    WindowProp m_window_property;

    EventStack<Layer *> m_layers;
    ImGuiLayer *m_imguiLayer;
};

}  // namespace SD

#endif /* SD_ENGINE_HPP */
