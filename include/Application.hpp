#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Window/Window.hpp"

namespace S2D {

class Application {
   public:
    Application();
    ~Application();

    Application(const Application &application) = delete;
    Application &operator=(const Application &application) = delete;

    bool init();
    void free();

    void run();

   private:
    Window m_window;
    bool m_isInit;
};

}  // namespace S2D

#endif /* APPLICATION_HPP */
