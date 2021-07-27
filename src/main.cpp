#include "Core/Engine.hpp"

int main(int, char **) {
    sd::Engine::instance().create();
    sd::Engine::instance().run();
    sd::Engine::instance().destroy();
    return 0;
}
