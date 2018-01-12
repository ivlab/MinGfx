
// The GraphicsApp base class can also run as its own app, but it just displays a blank screen

#include "GraphicsApp.h"

int main(int argc, char **argv) {

    cs3081::InitGraphics();

    cs3081::GraphicsApp *app = new cs3081::GraphicsApp(1024, 768, "3081 Graphics App");
    app->Run();

    cs3081::ShutdownGraphics();

    return 0;
}
