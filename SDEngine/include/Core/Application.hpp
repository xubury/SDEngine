#ifndef SD_APPLICATION_HPP
#define SD_APPLICATION_HPP

#include "Core/Vars.hpp"
#include "Core/Window.hpp"
#include "Core/ImGuiLayer.hpp"
#include "Graphics/Device.hpp"

int main(int argc, char **argv);

namespace SD {

class SD_CORE_API Application {
   public:
    void Shutdown();

    ImGuiLayer *GetImGuiLayer() const { return m_imgui; }

   protected:
    virtual void OnInit();
    virtual void OnDestroy();

    template <typename T>
    T *GetLayer(const std::string &name)
    {
        auto iter = std::find_if(
            m_layers.begin(), m_layers.end(),
            [&name](const auto &lhs) { return lhs->GetName() == name; });
        if (iter != m_layers.end()) {
            return static_cast<T *>(*iter);
        }
        return nullptr;
    }

    template <typename T, typename... ARGS>
    T *CreateLayer(ARGS &&...args)
    {
        T *layer = new T(std::forward<ARGS>(args)...);
        layer->SetAppVars(MakeAppVars());
        layer->OnInit();
        return layer;
    }

    void PushLayer(Layer *layer);

    void PushOverlay(Layer *layer);

    void PopLayer(Layer *layer);

    void DestroyLayer(Layer *layer);

    Application(const std::string &title, Device::API api);
    virtual ~Application();

    Application(const Application &application) = delete;
    Application &operator=(const Application &application) = delete;

    APP_VARS
    Scope<Window> m_window;

   private:
    SET_APP_VARS
    MAKE_APP_VARS

    friend int ::main(int argc, char **argv);

    void InitSettings();

    void Run();

    void Tick(float dt);

    void Render();

    EventStack<Layer *> m_layers;
    ImGuiLayer *m_imgui;

    HandlerRegistration m_quit_handler;
};

}  // namespace SD

#endif /* SD_APPLICATION_HPP */
