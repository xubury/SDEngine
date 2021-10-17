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
    template <typename LAYER, typename... ARGS>
    Ref<LAYER> pushLayer(ARGS &&...args) {
        auto layer =
            m_layers.pushLayer(createRef<LAYER>(std::forward<ARGS>(args)...));
        return std::static_pointer_cast<LAYER>(layer);
    }

    template <typename LAYER, typename... ARGS>
    Ref<LAYER> pushOverlay(ARGS &&...args) {
        auto layer =
            m_layers.pushOverlay(createRef<LAYER>(std::forward<ARGS>(args)...));
        return std::static_pointer_cast<LAYER>(layer);
    }

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

    Ref<Window> m_context;
    LayerStack m_layers;
    Ref<ImGuiLayer> m_imguiLayer;
    Ref<RenderEngine> m_renderEngine;
    Ref<InputEngine> m_inputEngine;
};

}  // namespace sd

#endif /* SD_ENGINE_HPP */
