#define SDL_MAIN_HANDLED

#include "Core/EntryPoint.hpp"

int main(int, char **) {
    auto app = SD::CreateApp();
    app->OnInit();
    app->OnStart();
    app->Run();
    app->OnExit();
    app->OnDestroy();
    delete app;
    return 0;
}
