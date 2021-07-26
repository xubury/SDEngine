#include "Application.hpp"

int main(int, char **) {
    sd::Application app;
    app.init();
    app.run();
    app.free();
    return 0;
}
