#define SDL_MAIN_HANDLED

#include "Core/EntryPoint.hpp"

int main(int, char **) {
    auto app = SD::CreateApp();
    app->OnStart();
    app->Run();
    app->OnExit();
    delete app;
    return 0;
}
