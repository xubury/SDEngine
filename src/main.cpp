#include "Core/Engine.hpp"

int main(int, char **) {
    sd::Engine engine;
    engine.init();
    engine.run();
    engine.free();
    return 0;
}
