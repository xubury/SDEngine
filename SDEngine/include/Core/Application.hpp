#ifndef SD_APPLICATION_HPP
#define SD_APPLICATION_HPP

#include "Core/Window.hpp"
#include "Core/ImGuiLayer.hpp"
#include "Graphics/Device.hpp"
#include "Utility/Ini.hpp"

int main(int argc, char **argv);

namespace SD {

#define IMPLEMENT_APP(X)                          \
    int main(int, char **)                        \
    {                                             \
        SD::Application::s_instance = new X;      \
        SD::Application::s_instance->OnInit();    \
        SD::Application::s_instance->Run();       \
        SD::Application::s_instance->OnDestroy(); \
        delete SD::Application::s_instance;       \
        return 0;                                 \
    }

class SD_CORE_API Application {
   public:
    template <typename T = Application>
    static T &GetApp()
    {
        return *dynamic_cast<T *>(s_instance);
    }

    void Shutdown();

    ImGuiLayer *GetImGuiLayer() const { return m_imgui; }

    Ini &GetSettings() { return m_settings; };

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
        layer->m_dispatcher = m_dispatcher;
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

    Scope<Window> m_window;

   private:
    friend int ::main(int argc, char **argv);

    void InitSettings();

    void Run();

    void Tick(float dt);

    void Render();

    EventStack<Layer *> m_layers;
    Ref<EventDispatcher> m_dispatcher;
    ImGuiLayer *m_imgui;

    Ini m_settings;

    HandlerRegistration m_quit_handler;
    static Application *s_instance;

    std::filesystem::path GetAppDirectory();
};

}  // namespace SD

#endif /* SD_APPLICATION_HPP */
