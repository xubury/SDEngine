#ifndef SD_ENGINE_HPP
#define SD_ENGINE_HPP

#include "Core/Window.hpp"

namespace sd {

class SD_API Engine {
   public:
    Engine();
    ~Engine();

    Engine(const Engine &application) = delete;
    Engine &operator=(const Engine &application) = delete;

    bool init();
    void free();

    void run();

   private:
    void processEvent(const SDL_Event &event);

    bool m_isInit;
    Window m_window;
};

}  // namespace sd

#endif /* SD_ENGINE_HPP */
