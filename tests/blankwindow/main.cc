
// The GraphicsApp base class can also run as its own app, but it just displays a blank screen

#include <mingfx.h>

int main(int argc, char **argv) {

    MinGfx::InitGraphics();

    GraphicsApp *app = new GraphicsApp(1024, 768, "A Blank Window!");
    app->Run();

    MinGfx::ShutdownGraphics();

    return 0;
}
