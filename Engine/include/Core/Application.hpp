#ifndef SD_ENGINE_HPP
#define SD_ENGINE_HPP

#include "Core/Vars.hpp"
#include "Core/Window.hpp"
#include "Core/EventStack.hpp"
#include "ImGui/ImGuiLayer.hpp"

int main(int argc, char **argv);

namespace SD {

class Application;

Application &GetApp();

std::filesystem::path GetAppDirectory();

class SD_API Application {
   public:
    void Quit();

    std::filesystem::path GetDirectory();

    Application(const Application &application) = delete;
    Application &operator=(const Application &application) = delete;

    Window &GetWindow();

   protected:
    void PushLayer(Layer *layer);

    void PushOverlay(Layer *layer);

    void PopLayer(Layer *layer);

    void DestroyLayer(Layer *layer);

    Application();
    virtual ~Application();

    APP_VARS
   private:
    SET_APP_VARS
    MAKE_APP_VARS

    friend int ::main(int argc, char **argv);

    virtual void OnInit() = 0;

    void Run();

    void ProcessEvent(const SDL_Event &event);

    void ProcessEvents();

    void Tick(float dt);
    void Render();

    Ref<Window> m_window;

    EventStack<Layer *> m_layers;
    ImGuiLayer *m_imguiLayer;
};

}  // namespace SD

#endif /* SD_ENGINE_HPP */
