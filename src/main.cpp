#include "Application.hpp"

int main(int, char **) {
    S2D::Application app;
    app.init();
    app.run();
    app.free();
    return 0;
}
