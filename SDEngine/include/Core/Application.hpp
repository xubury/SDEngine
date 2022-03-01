#ifndef SD_APPLICATION_HPP
#define SD_APPLICATION_HPP

#include "Core/Vars.hpp"
#include "Core/Window.hpp"
#include "Core/ImGuiLayer.hpp"

int main(int argc, char **argv);

namespace SD {

class SD_CORE_API Application {
   public:
    void Quit();

    Window &GetWindow() { return *m_window; }

   protected:
    virtual void OnInit();
    virtual void OnDestroy();

    ImGuiLayer *GetImGuiLayer() const { return m_imgui.get(); }

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
    Ref<T> CreateLayer(ARGS &&...args) {
        Ref<T> layer = CreateRef<T>(std::forward<ARGS>(args)...);
        layer->SetAppVars(MakeAppVars());
        layer->OnInit();
        return layer;
    }

    void PushLayer(const Ref<Layer> &layer);

    void PushOverlay(const Ref<Layer> &layer);

    void PopLayer(const Ref<Layer> &layer);

    Application(const std::string &title, GraphicsAPI api);
    virtual ~Application() = default;

    Application(const Application &application) = delete;
    Application &operator=(const Application &application) = delete;

    APP_VARS
   private:
    SET_APP_VARS
    MAKE_APP_VARS

    friend int ::main(int argc, char **argv);

    void Run();

    void ProcessEvent(const Event &event);

    void Tick(float dt);
    void Render();

    EventStack<Ref<Layer>> m_layers;
    Ref<ImGuiLayer> m_imgui;

    Ref<Window> m_window;
};

}  // namespace SD

#endif /* SD_APPLICATION_HPP */
