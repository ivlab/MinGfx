
#include <nanogui/nanogui.h>
#include <iostream>


namespace cs3081 {

/// Call this function once before creating an instance of GraphicsApp
void InitGraphics();

/// Call this function after deleting all instances of GraphicsApp
void ShutdownGraphics();



/** Base class for several cs3081 class projects that make use of graphics.
    The class uses the nanogui and nanovg libraries to support creating
    simple graphical user interfaces and drawing simple 2D graphics inside
    an OpenGL graphics context.  nanogui has several additional dependencies
    including GLFW, which helps to open a graphics window and setup the
    OpenGL rendering context.
**/
class GraphicsApp : public nanogui::Screen {
public:

    /// cs3081::InitGraphics() must be called before this constructor!
    GraphicsApp(int width, int height, const std::string &caption);


    virtual ~GraphicsApp();


    // Callback methods -- override these and fill in to respond to user
    // input events.

    virtual void OnMouseMove(int x, int y) {}

    virtual void OnLeftMouseDown(int x, int y) {}

    virtual void OnLeftMouseUp(int x, int y) {}

    virtual void OnRightMouseDown(int x, int y) {}

    virtual void OnRightMouseUp(int x, int y) {}

    virtual void OnKeyDown(const char *c, int modifiers) {}

    virtual void OnKeyUp(const char *c, int modifiers) {}

    virtual void OnSpecialKeyDown(int key, int scancode, int modifiers) {}

    virtual void OnSpecialKeyUp(int key, int scancode, int modifiers) {}


    /// Called once per frame.  dt is the elapsed time since the last call.
    /// Override this and fill it in to update your simulation code or any
    /// other updates you need to make to your model that are timed rather
    /// than in response to user input.
    virtual void UpdateSimulation(double dt) {}

    /// Override this to draw graphics using the nanovg vector graphics
    /// library, which provides an easy way to draw 2D shapes to the screen.
    virtual void DrawUsingNanoVG(NVGcontext *ctx) {}

    /// Override this to draw graphics using raw OpenGL 2D or 3D graphics
    /// calls.
    virtual void DrawUsingOpenGL() {}

    /// After creating a new GraphicsApp, call this to start the app's
    /// mainloop.  Each time through the mainloop the app will: 1. respond
    /// any user input events by calling the On*() callback methods, 2. call
    /// UpdateSimulation(), and 3. call the two Draw*() methods.  Note that
    /// Run() does not return until the user closes the app and the program
    /// is ready to shutdown.
    void Run();


private:

    bool mouseButtonEvent(const Eigen::Vector2i &p, int button, bool down, int modifiers);
    bool mouseMotionEvent(const Eigen::Vector2i &p, const Eigen::Vector2i &rel, int button, int modifiers);
    bool keyboardEvent(int key, int scancode, int action, int modifiers);
    void draw(NVGcontext *ctx);
    void drawContents();

    double lastDrawT_;
};



} // end cs3081 namespace
