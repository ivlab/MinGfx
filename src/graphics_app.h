
#ifndef SRC_GRAPHICS_APP_H_
#define SRC_GRAPHICS_APP_H_

#include <nanogui/nanogui.h>
#include <iostream>

#include "point2.h"
#include "vector2.h"

namespace MinGfx {


/** Base class for several cs3081 class projects that make use of graphics.
    The class uses the nanogui and nanovg libraries to support creating
    simple graphical user interfaces and drawing simple 2D graphics inside
    an OpenGL graphics context.  nanogui has several additional dependencies
    including GLFW, which helps to open a graphics window and setup the
    OpenGL rendering context.
**/
class GraphicsApp {
public:

    /// cs3081::InitGraphics() must be called before this constructor!
    GraphicsApp(int width, int height, const std::string &caption);


    virtual ~GraphicsApp();


    // Callback methods -- override these and fill in to respond to user
    // input events.

    virtual void OnMouseMove(const Point2 &pos, const Vector2 &delta) {}

    virtual void OnLeftMouseDown(const Point2 &pos) {}
    virtual void OnLeftMouseDrag(const Point2 &pos, const Vector2 &delta) {}
    virtual void OnLeftMouseUp(const Point2 &pos) {}

    virtual void OnMiddleMouseDown(const Point2 &pos) {}
    virtual void OnMiddleMouseDrag(const Point2 &pos, const Vector2 &delta) {}
    virtual void OnMiddleMouseUp(const Point2 &pos) {}
    
    virtual void OnRightMouseDown(const Point2 &pos) {}
    virtual void OnRightMouseDrag(const Point2 &pos, const Vector2 &delta) {}
    virtual void OnRightMouseUp(const Point2 &pos) {}

    /// Transforms a keyboard down event into the actual character typed.
    /// Modifiers are defined here: http://www.glfw.org/docs/latest/group__mods.html
    virtual void OnKeyDown(const char *c, int modifiers) {}

    /// Transforms a keyboard down event into the actual character typed.
    /// Modifiers are defined here: http://www.glfw.org/docs/latest/group__mods.html
    virtual void OnKeyUp(const char *c, int modifiers) {}

    /// The values for key, scancode, and modifiers are documented here:
    /// http://www.glfw.org/docs/latest/group__keys.html
    virtual void OnSpecialKeyDown(int key, int scancode, int modifiers) {}

    /// The values for key, scancode, and modifiers are documented here:
    /// http://www.glfw.org/docs/latest/group__keys.html
    virtual void OnSpecialKeyUp(int key, int scancode, int modifiers) {}


    /// After creating a new GraphicsApp, call this to start the app's
    /// mainloop.  Each time through the mainloop the app will: 1. respond
    /// any user input events by calling the On*() callback methods, 2. call
    /// UpdateSimulation(), and 3. call the two Draw*() methods.  Note that
    /// Run() does not return until the user closes the app and the program
    /// is ready to shutdown.
    void Run();
    
    
    /// Called once per frame.  dt is the elapsed time since the last call.
    /// Override this and fill it in to update your simulation code or any
    /// other updates you need to make to your model that are timed rather
    /// than in response to user input.
    virtual void UpdateSimulation(double dt) {}

    
    /// Override this to draw graphics using the nanovg vector graphics
    /// library, which provides an easy way to draw 2D shapes to the screen.
    virtual void DrawUsingNanoVG(NVGcontext *ctx) {}

    /// Override this to initialize the OpenGL context with textures, vertex
    /// buffers, etc. that you will use later inside DrawUsingOpenGL().  This
    /// InitOpenGL function is called once on program startup just after the
    /// OpenGL drawing context is created.
    virtual void InitOpenGL() {}
    
    /// Override this to draw graphics using raw OpenGL 2D or 3D graphics
    /// calls.
    virtual void DrawUsingOpenGL() {}

    
    /// True if the specified is is currently held down.  Uses the GLFW
    /// key codes found here: http://www.glfw.org/docs/latest/group__keys.html
    bool is_key_down(int key);
    
    /// True if the left mouse button is currently held down.
    bool is_left_mouse_down();
    
    /// True if the middle mouse button is currently held down.
    bool is_middle_mouse_down();
    
    /// True if the right mouse button is currently held down.
    bool is_right_mouse_down();
    
    /// Returns the current width of the client area of the window in pixels
    int window_width();
    
    /// Returns the current height of the client area of the window in pixels
    int window_height();
    
    /// Returns the current width of the framebuffer in pixels.  Note that on
    /// some displays (e.g., Mac Retina) the framebuffer is larger than the
    /// window.
    int framebuffer_width();
    
    /// Returns the current height of the framebuffer in pixels.  Note that on
    /// some displays (e.g., Mac Retina) the framebuffer is larger than the
    /// window.
    int framebuffer_height();

    /// Returns width/height for the current shape of the window
    float aspect_ratio();
    
    
    /// Transforms a point in viewport coordinates (pixels where top left = (0,0)
    /// and bottom right = (window width-1, window height-1)) to normalized
    /// device coordinates, (top left = (-1,1) bottom right (1,-1)).
    Point2 pixels_to_normalized_coordinates(Point2 pointInPixels);
    
    /// Transforms a point in normalized device coordinates (top left = (-1,1)
    /// bottom right (1,-1)) to pixels (top left = (0,0), bottom right =
    /// (window width-1, window height-1))
    Point2 normalized_coordinates_to_pixels(Point2 pointInNDC);
    
    
    /// Transforms a vector in viewport coordinates (pixels where top left = (0,0)
    /// and bottom right = (window width-1, window height-1)) to normalized
    /// device coordinates, (top left = (-1,1) bottom right (1,-1)).
    Vector2 pixels_to_normalized_coordinates(Vector2 vectorInPixels);
    
    /// Transforms a vector in normalized device coordinates (top left = (-1,1)
    /// bottom right (1,-1)) to pixels (top left = (0,0), bottom right =
    /// (window width-1, window height-1))
    Vector2 normalized_coordinates_to_pixels(Vector2 pointInNDC);
    
    /// Returns the z buffer value under the specified pixel.  z will be -1 at
    /// the near plane and +1 at the far plane.
    float z_value_at_pixel(const Point2 &pointInPixels, unsigned int whichBuffer = GL_BACK);

    /// Access to the underlying NanoGUI Screen object
    nanogui::Screen* screen();

    /// Access to the underlying GLFWwindow object
    GLFWwindow* window();

private:
    
    bool cursor_pos_glfw_cb(double x, double y);
    bool mouse_button_glfw_cb(int button, int action, int modifiers);
    bool key_glfw_cb(int key, int scancode, int action, int mods);
    bool char_glfw_cb(unsigned int codepoint);
    bool drop_glfw_cb(int count, const char **filenames);
    bool scroll_glfw_cb(double x, double y);
    bool resize_glfw_cb(int width, int height);
    
    nanogui::Screen *screen_;
    GLFWwindow* window_;
    double lastDrawT_;
    Point2 lastMouse_;
    bool leftDown_;
    bool middleDown_;
    bool rightDown_;
};


} // end namespace

#endif

