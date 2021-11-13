#define SDL_MAIN_HANDLED

#include "Core/EntryPoint.hpp"

int main(int, char **) {
    auto app = SD::createApp();
    app->onInit();
    app->run();
    app->destroy();
    return 0;
}
