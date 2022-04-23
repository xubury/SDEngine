#ifndef SD_APPLICATION_HPP
#define SD_APPLICATION_HPP

#include "Core/Window.hpp"
#include "Core/ImGuiLayer.hpp"
#include "Core/GraphicsLayer.hpp"
#include "Core/InputDevice.hpp"
#include "ECS/SceneManager.hpp"
#include "Resource/ResourceManager.hpp"
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
        return layer;
    }

    void PushLayer(Layer *layer);

    void PushOverlay(Layer *layer);

    void PopLayer(Layer *layer);

    void DestroyLayer(Layer *layer);

    void CreateImGuiLayer();

    Application(const std::string &title, Device::API api);
    virtual ~Application();

    Application(const Application &application) = delete;
    Application &operator=(const Application &application) = delete;

    Scope<Window> m_window;
    Scope<Device> m_device;

    GraphicsLayer *m_graphics_layer;

    ResourceManager m_resources;
    SceneManager m_scenes;
    InputDevice m_input;

   private:
    ImGuiLayer *m_imgui_layer;

    friend int ::main(int argc, char **argv);

    void LoadSettingsFile();

    void Run();

    void Tick(float dt);

    void Render();

    EventStack<Layer *> m_layers;

    Ini m_settings;

    static Application *s_instance;

    std::filesystem::path GetAppDirectory();
};

}  // namespace SD

#endif /* SD_APPLICATION_HPP */
