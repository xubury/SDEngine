#include "Core/Engine.hpp"

int main(int, char **) {
    sd::Engine::instance().init();
    sd::Engine::instance().run();
    sd::Engine::instance().free();
    return 0;
}
