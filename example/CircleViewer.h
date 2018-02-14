
#include <mingfx.h>


/** An application that opens up a window that includes a few buttons 
    for controlling the simulation and can be used to draw circles and 
    other computer graphics.

    After constructing a new CircleViewer, call Run() to start and
    run the application.  Run() will not return until the application
    window is closed.  Make sure that you call cs3081::InitGraphics()
    before creating the RobotViewer app.  Example:

    ```
    int main(int argc, char **argv) {
        cs3081::InitGraphics();
        cs3081::CircleViewer *app = new cs3081::CircleViewer();
        app->Run();
        cs3081::ShutdownGraphics();
        return 0;
    }
    ```

    While the window is open UpdateSimulation() will be called
    repeatedly, once per frame.  Fill this in to update your simulation
    or perform any other processing that should happen over time as the
    simulation progresses.

    Fill in the On*() methods as desired to respond to user input events.

    Fill in the Draw*() methods to draw graphics to the screen using
    either the nanovg library or raw OpenGL.
*/
class CircleViewer : public GraphicsApp {
public:
    CircleViewer();
    ~CircleViewer();

    void InitNanoGUI();

    void UpdateSimulation(double dt);


    void OnRestartBtnPressed();

    void OnPauseBtnPressed();


    void OnMouseMove(const Point2 &pos, const Vector2 &delta);

    void OnLeftMouseDown(const Point2 &pos);
    void OnLeftMouseDrag(const Point2 &pos, const Vector2 &delta);
    void OnLeftMouseUp(const Point2 &pos);
    
    void OnRightMouseDown(const Point2 &pos);
    void OnRightMouseDrag(const Point2 &pos, const Vector2 &delta);
    void OnRightMouseUp(const Point2 &pos);
    
    
    void OnKeyDown(const char *c, int modifiers);

    void OnKeyUp(const char *c, int modifiers);

    void OnSpecialKeyDown(int key, int scancode, int modifiers);

    void OnSpecialKeyUp(int key, int scancode, int modifiers);


    void DrawUsingNanoVG(NVGcontext *ctx);

    void DrawUsingOpenGL();

private:
    double simTime_;
    bool paused_;
    nanogui::Button *pauseBtn_;
};
