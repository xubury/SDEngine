#ifndef SD_ENGINE_HPP
#define SD_ENGINE_HPP

#include "Core/Window.hpp"

namespace sd {

class SD_API Engine {
   public:
    static Engine &instance() {
        static Engine s_instance;
        return s_instance;
    }

    Engine(const Engine &application) = delete;
    Engine &operator=(const Engine &application) = delete;

    bool create();
    void destroy();

    void run();

   private:
    Engine();
    ~Engine();

    void processEvent(const SDL_Event &event);
    void tick();
    void render();

    bool m_isInit;
    Window m_window;
};

}  // namespace sd

#endif /* SD_ENGINE_HPP */
