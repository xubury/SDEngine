#define SDL_MAIN_HANDLED

#include "Core/EntryPoint.hpp"

int main(int, char **) {
    auto app = SD::createApp();
    app->onInit();
    app->run();
    delete app;
    return 0;
}
