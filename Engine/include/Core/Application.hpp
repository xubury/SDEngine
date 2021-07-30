#ifndef SD_ENGINE_HPP
#define SD_ENGINE_HPP

#include "Core/Window.hpp"
#include "Core/LayerStack.hpp"

namespace sd {

class SD_API Application {
   public:
    static Application &instance();

    Application(const Application &application) = delete;
    Application &operator=(const Application &application) = delete;

    bool init();
    void destroy();

    void pushLayer(Ref<Layer> layer);

    void run();

   protected:
    Application();
    ~Application();

   private:
    static Application *s_instance;

    void onEventPoll(const SDL_Event &event);

    void tick(float dt);
    void render();

    bool m_isInit;
    Window m_window;
    LayerStack m_layers;
};

}  // namespace sd

#endif /* SD_ENGINE_HPP */
