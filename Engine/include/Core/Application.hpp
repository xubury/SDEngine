#ifndef SD_ENGINE_HPP
#define SD_ENGINE_HPP

#include "Core/Vars.hpp"
#include "Core/Window.hpp"
#include "Core/ImGuiLayer.hpp"

int main(int argc, char **argv);

namespace SD {

class SD_CORE_API Application {
   public:
    void Quit();

   protected:
    ImGuiLayer *GetImGuiLayer() const { return m_imguiLayer; }

    template <typename T>
    T *GetLayer(const std::string &name) {
        auto iter = std::find_if(
            m_layers.begin(), m_layers.end(),
            [&name](const auto &lhs) { return lhs->GetName() == name; });
        if (iter != m_layers.end()) {
            return static_cast<T *>(*iter);
        }
        return nullptr;
    }

    template <typename T, typename... ARGS>
    T *CreateLayer(ARGS &&...args) {
        T *layer = new T(std::forward<ARGS>(args)...);
        layer->SetAppVars(MakeAppVars());
        layer->OnInit();
        return layer;
    }

    void PushLayer(Layer *layer);

    void PushOverlay(Layer *layer);

    void PopLayer(Layer *layer);

    void DestroyLayer(Layer *layer);

    Application(const std::string &title);
    virtual ~Application();

    Application(const Application &application) = delete;
    Application &operator=(const Application &application) = delete;

    APP_VARS
   private:
    SET_APP_VARS
    MAKE_APP_VARS

    friend int ::main(int argc, char **argv);

    virtual void OnStart() = 0;
    virtual void OnExit() = 0;

    void Run();

    void ProcessEvent(const SDL_Event &event);

    void ProcessEvents();

    void Tick(float dt);
    void Render();

    EventStack<Layer *> m_layers;
    ImGuiLayer *m_imguiLayer;
};

}  // namespace SD

#endif /* SD_ENGINE_HPP */
