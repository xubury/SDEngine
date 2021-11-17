#define SDL_MAIN_HANDLED

#include "Core/EntryPoint.hpp"

int main(int, char **) {
    auto app = SD::CreateApp();
    app->OnInit();
    app->Run();
    delete app;
    return 0;
}
