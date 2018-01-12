
#include "CircleViewer.h"


int main(int argc, char **argv) {

    cs3081::InitGraphics();

    cs3081::CircleViewer *app = new cs3081::CircleViewer();
    app->Run();

    cs3081::ShutdownGraphics();

    return 0;
}

