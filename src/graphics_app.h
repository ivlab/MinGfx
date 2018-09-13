/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2017, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	... 
 */


#ifndef SRC_GRAPHICS_APP_H_
#define SRC_GRAPHICS_APP_H_

#include <nanogui/nanogui.h>
#include <iostream>

#include "point2.h"
#include "vector2.h"

/** Namespace for the MinGfx Toolkit */
namespace mingfx {


/** This is the main application base class for the MinGfx Toolkit.  
 
 _Create a Subclass:_
 
 To create your own graphics application, you should create a subclass of GraphicsApp
 and then override some key functions:
 
 1. User Input: To get input from the keyboard and mouse, override OnMouseMove() and/or the other On...() functions.

 2. Drawing Graphics: To draw graphics override one or more of the Draw*() functions.
 
    - DrawUsingNanoVG() is the right place to make 2D drawing calls using the nanovg library.
 
    - DrawUsingOpenGL() is the right place to make 2D or 3D drawing calls using OpenGL.  This includes drawing using the Mesh, QuickShapes, DefaultShader, ShaderProgram, and all other MinGfx classes since these are all based on OpenGL.
 
    - InitOpenGL() is the right place to load textures, meshes, shaders, and other graphics objects that can only be created after the OpenGL context exists.
 
 3. Physics, Animation, AI, etc.: Override the UpdateSimulation() function to do other non-graphics calculations required by your program.  This is called automatically once per frame.
 
 Keep in mind that internally the app uses a rendering loop that looks something like this:
 ~~~
 InitOpenGL(); // your hook for initialization after OpenGL is active
 while (!program_ready_to_close) {
     // user input
     internal_get_input_events_from_operating_system();
     OnMouseMove();  // your hook for processing input
     On*(); // all other event callbacks -- your hook for processing input
 
     // phyics, etc.
     UpdateSimulation(); // your hook for physics, animation, AI, etc.
 
     // draw graphics
     internal_render_gui_elements_using_nanogui();
     DrawUsingNanoVG(); // your hook for drawing 2D vector graphics
     DrawUsingOpenGL(); // your hook for 2D/3D rendering with OpenGL
 }
 ~~~
 
 _A Complete Example with GUI Widgets_
 
 If you wish to add some buttons, sliders, etc. in your application, you can do this inside GraphicsApp by accessing the NanoGUI library.  You will need to pass NanoGUI a nanogui::screen object, which you can get from the screen() function.  NanoGui setup should be done in the constructor, like this:
 ~~~
 #include <mingfx.h>
 
 class MyApp : public GraphcisApp {
 public:
    MyApp() : GraphicsApp(1024,768, "My Amazing App") {
    }
    
    virtual ~MyApp() {}

    void InitNanoGUI() {
        // Setup the GUI window
        nanogui::Window *window = new nanogui::Window(screen(), "My GUI Panel");
        window->setPosition(Eigen::Vector2i(10, 10));
        window->setSize(Eigen::Vector2i(400,200));
        window->setLayout(new nanogui::GroupLayout());
 
        nanoguiButton pause_btn = new nanogui::Button(window, "Pause");
        pause_btn->setCallback(std::bind(&QuakeApp::OnPauseBtnPressed, this));
        pause_btn->setTooltip("Toggle playback.");
 
        screen()->performLayout();
    }
 
    void OnMouseMove(const Point2 &pos, const Vector2 &delta) {
        std::cout << "Mouse moved to " << pos << std::endl;
    }
 
    void InitOpenGL() {
        glClearColor(0.0, 0.0, 0.0, 1);
    }
 
    void DrawUsingOpenGL() {
        Matrix4 model = Matrix4::Translation(Vector3(-1,0,0)) * Matrix4::Scale(Vector3(0.5, 0.5, 0.5));
        Matrix4 view = Matrix4::LookAt(Point3(0,0,3), Point3(0,0,0), Vector3(0,1,0));
        Matrix4 proj = Matrix4::Perspective(60.0, aspect_ratio(), 0.1, 10.0);
        quick_shapes_.DrawCube(model, view, proj, Color(1,1,1));
    }
 
 private:
    QuickShapes quick_shapes_;
 };
 
 
 int main(int argc, const char *argv[]) {
    MyApp app;
    app.Run();
    return 0;
 }
 ~~~
 
 */
class GraphicsApp {
public:

    /** \brief Constructs a new app but does not yet run it.
     
     \param width The width of the client area of the window in pixels.
     \param height The height of the client area of the window in pixels.
     \param caption The caption for the window's title bar.
     \param init_graphics_in_constructor Defaults to true for backward compatibility,
            but new apps should set this to false.  This option was added to support
            testing graphics functions in batch mode with an off-screen framebuffer.
     */
    GraphicsApp(int width, int height, const std::string &caption);


    /// The destructor will shutdown the graphics system and window
    virtual ~GraphicsApp();


    // Callback methods -- override these and fill in to respond to user
    // input events.

    /** If the mouse has moved in the past frame and no mouse buttons are currently
     pressed, then this callback function will be called to report the new position
     of the mouse to you.
     
     \param pos This is the current position of the mouse in pixels, where (0,0)
     is at the top left corner of the screen and (window_width(), window_height())
     is the bottom right corner.
     
     \param delta This is the change in the position of the mouse in pixels since
     the last frame.
     */
    virtual void OnMouseMove(const Point2 &pos, const Vector2 &delta) {}

    /** If the mouse button was pressed down since the last frame, then this function
     will be called to notify you.
     
     \param pos This is the current position of the mouse in pixels, where (0,0)
     is at the top left corner of the screen and (window_width(), window_height())
     is the bottom right corner.
     */
    virtual void OnLeftMouseDown(const Point2 &pos) {}
    
    /** If the mouse button is held down and the mouse has moved in the past frame
     then this function will be called to tell you that a "dragging" operation is
     happening.
     
     \param pos This is the current position of the mouse in pixels, where (0,0)
     is at the top left corner of the screen and (window_width(), window_height())
     is the bottom right corner.
     
     \param delta This is the change in the position of the mouse in pixels since
     the last frame.
     */
    virtual void OnLeftMouseDrag(const Point2 &pos, const Vector2 &delta) {}
    
    /** If the mouse button was released since the last frame, then this function
     will be called to notify you.
     
     \param pos This is the current position of the mouse in pixels, where (0,0)
     is at the top left corner of the screen and (window_width()-1, window_height()-1)
     is the bottom right corner.
     */
    virtual void OnLeftMouseUp(const Point2 &pos) {}

    
    /// \copydoc GraphicsApp::OnLeftMouseDown(const Point2 &pos)
    virtual void OnMiddleMouseDown(const Point2 &pos) {}
    
    /// \copydoc GraphicsApp::OnLeftMouseDrag(const Point2 &pos, const Vector2 &delta)
    virtual void OnMiddleMouseDrag(const Point2 &pos, const Vector2 &delta) {}

    /// \copydoc GraphicsApp::OnLeftMouseUp(const Point2 &pos)
    virtual void OnMiddleMouseUp(const Point2 &pos) {}
    
    
    /// \copydoc GraphicsApp::OnLeftMouseDown(const Point2 &pos)
    virtual void OnRightMouseDown(const Point2 &pos) {}

    /// \copydoc GraphicsApp::OnLeftMouseDrag(const Point2 &pos, const Vector2 &delta)
    virtual void OnRightMouseDrag(const Point2 &pos, const Vector2 &delta) {}
    
    /// \copydoc GraphicsApp::OnLeftMouseUp(const Point2 &pos)
    virtual void OnRightMouseUp(const Point2 &pos) {}

    
    /** Transforms a keyboard down event into the actual character typed.
     \param c The character for the key that was pressed.
     \param modifiers If any modifiers (Alt, Ctrl, Shift, etc.) were held
     at the same time, then these are encoded in this int.  See the detailed
     description here: http://www.glfw.org/docs/latest/group__mods.html
     */
    virtual void OnKeyDown(const char *c, int modifiers) {}

    /** Transforms a keyboard repeat event into the actual character typed.
     \param c The character for the key that was pressed.
     \param modifiers If any modifiers (Alt, Ctrl, Shift, etc.) were held
     at the same time, then these are encoded in this int.  See the detailed
     description here: http://www.glfw.org/docs/latest/group__mods.html
     */
    virtual void OnKeyRepeat(const char *c, int modifiers) {}
    
    /** Transforms a keyboard up event into the actual character typed.
     \param c The character for the key that was pressed.
     \param modifiers If any modifiers (Alt, Ctrl, Shift, etc.) were held
     at the same time, then these are encoded in this int.  See the detailed
     description here: http://www.glfw.org/docs/latest/group__mods.html
     */
    virtual void OnKeyUp(const char *c, int modifiers) {}

    
    
    /// The values for key, scancode, and modifiers are documented here:
    /// http://www.glfw.org/docs/latest/group__keys.html
    virtual void OnSpecialKeyDown(int key, int scancode, int modifiers) {}

    /// The values for key, scancode, and modifiers are documented here:
    /// http://www.glfw.org/docs/latest/group__keys.html
    virtual void OnSpecialKeyRepeat(int key, int scancode, int modifiers) {}
    
    /// The values for key, scancode, and modifiers are documented here:
    /// http://www.glfw.org/docs/latest/group__keys.html
    virtual void OnSpecialKeyUp(int key, int scancode, int modifiers) {}


    /// Override this to respond when the graphics window and/or framebuffer
    /// are resized, either by the user dragging the window or through a call
    /// to ResizeWindow().
    virtual void OnWindowResize(int new_width, int new_height) {}
    
    
    /** After creating a new GraphicsApp, call this to start the app's
     mainloop.  Each time through the mainloop the app will: 1. respond
     any user input events by calling the On*() callback methods, 2. call
     UpdateSimulation(), and 3. call the two Draw*() methods.  Note that
     Run() does not return until the user closes the app and the program
     is ready to shutdown.
     */
    virtual void Run();

    
    /** Called at the beginning of the Run() method.  Override this to initialize
      any NanoGUI graphics related properties including 2D windows, buttons,
      sliders, etc...

     IMPORTANT: Put any NanoGUI initialization code here, NOT in the constructors
     of the classes that you create, or, create your classes from within this
     function.  The graphics calls will fail if the OpenGL context has not yet
     been initialized, and it is not guaranteed to be initialized until this
     function has been called.
     */
    virtual void InitNanoGUI() {}
    
    /** Override this to initialize the OpenGL context with textures, vertex
     buffers, etc. that you will use later inside DrawUsingOpenGL().  This
     InitOpenGL() function is called once on program startup just after the
     OpenGL drawing context is created.

     IMPORTANT: Put any OpenGL initialization code here, NOT in the constructors
     of the classes that you create, or, create your classes from within this
     function.  The graphics calls will fail if the OpenGL context has not yet
     been initialized, and it is not guaranteed to be initialized until this
     function has been called.
     */
    virtual void InitOpenGL() {}


    /** Called once per frame.  Override this and fill it in to update your 
     simulation code or any other updates you need to make to your model that 
     are timed rather than in response to user input.
     
     \param dt is the elapsed time since the last call.
     */
    virtual void UpdateSimulation(double dt) {}

    
    /// Override this to draw graphics using the nanovg vector graphics
    /// library, which provides an easy way to draw 2D shapes to the screen.
    virtual void DrawUsingNanoVG(NVGcontext *ctx) {}


    
    /// Override this to draw graphics using raw OpenGL 2D or 3D graphics
    /// calls.
    virtual void DrawUsingOpenGL() {}

    
    /// True if the specified is is currently held down.  Uses the GLFW
    /// key codes found here: http://www.glfw.org/docs/latest/group__keys.html
    virtual bool IsKeyDown(int key);
    
    /// True if the left mouse button is currently held down.
    virtual bool IsLeftMouseDown();
    
    /// True if the middle mouse button is currently held down.
    virtual bool IsMiddleMouseDown();
    
    /// True if the right mouse button is currently held down.
    virtual bool IsRightMouseDown();
    
    /// Returns the current width of the client area of the window in pixels
    virtual int window_width();
    
    /// Returns the current height of the client area of the window in pixels
    virtual int window_height();
    
    /** Returns the current width of the framebuffer in pixels.  Note that on
     some displays (e.g., Mac Retina) the framebuffer is larger than the
     window.
     */
    virtual int framebuffer_width();
    
    /** Returns the current height of the framebuffer in pixels.  Note that on
     some displays (e.g., Mac Retina) the framebuffer is larger than the
     window.
     */
    virtual  int framebuffer_height();

    /// Returns width/height for the current shape of the window
    virtual float aspect_ratio();
    
    
    /** Transforms a point in viewport coordinates (pixels where top left = (0,0)
     and bottom right = (window_width()-1, window_height()-1)) to normalized
     device coordinates, (top left = (-1,1) bottom right (1,-1)).
     */
    virtual Point2 PixelsToNormalizedDeviceCoords(const Point2 &pointInPixels);
    
    /** Transforms a point in normalized device coordinates (top left = (-1,1)
     bottom right (1,-1)) to pixels (top left = (0,0), bottom right =
     (window width-1, window height-1))
     */
    virtual Point2 NormalizedDeviceCoordsToPixels(const Point2 &pointInNDC);
    
    
    /** Transforms a vector in viewport coordinates (pixels where top left = (0,0)
     and bottom right = (window width-1, window height-1)) to normalized
     device coordinates, (top left = (-1,1) bottom right (1,-1)).
     */
    virtual Vector2 PixelsToNormalizedDeviceCoords(const Vector2 &vectorInPixels);
    
    /** Transforms a vector in normalized device coordinates (top left = (-1,1)
     bottom right (1,-1)) to pixels (top left = (0,0), bottom right =
     (window width-1, window height-1))
     */
    virtual Vector2 NormalizedDeviceCoordsToPixels(const Vector2 &pointInNDC);
    
    /// Returns the z buffer value under the specified pixel.  z will be 0 at
    /// the near plane and +1 at the far plane.
    virtual float ReadZValueAtPixel(const Point2 &pointInPixels, unsigned int whichBuffer = GL_BACK);

    /// Access to the underlying NanoGUI Screen object
    virtual nanogui::Screen* screen();

    /// Access to the underlying GLFWwindow object
    virtual GLFWwindow* window();


    /// Cause the graphics windows to resize programmatically rather than by dragging
    /// on the corner manually.
    virtual void ResizeWindow(int new_width, int new_height);
    
    
    /** Users cannot make any graphics calls (e.g., setting the clear color,
     saving mesh data to the GPU) until the graphics context is initialized
     by calling this method.  It is called automatically by the Run() method
     before calling the InitNanoGUI() and InitOpenGL() methods.  So, users
     should place all of their graphics initialization code inside one of
     those two methods.
     */
    virtual void InitGraphicsContext();

private:

    bool cursor_pos_glfw_cb(double x, double y);
    bool mouse_button_glfw_cb(int button, int action, int modifiers);
    bool key_glfw_cb(int key, int scancode, int action, int mods);
    bool char_glfw_cb(unsigned int codepoint);
    bool drop_glfw_cb(int count, const char **filenames);
    bool scroll_glfw_cb(double x, double y);
    bool resize_glfw_cb(int width, int height);
    
    virtual void mouse_move(const Point2 &pos, const Vector2 &delta) {
        OnMouseMove(pos, delta);
    }
    virtual void left_mouse_down(const Point2 &pos) {
        OnLeftMouseDown(pos);
    }
    virtual void left_mouse_drag(const Point2 &pos, const Vector2 &delta) {
        OnLeftMouseDrag(pos, delta);
    }
    virtual void left_mouse_up(const Point2 &pos) {
        OnLeftMouseUp(pos);
    }
    virtual void middle_mouse_down(const Point2 &pos) {
        OnMiddleMouseDown(pos);
    }
    virtual void middle_mouse_drag(const Point2 &pos, const Vector2 &delta) {
        OnMiddleMouseDrag(pos, delta);
    }
    virtual void middle_mouse_up(const Point2 &pos) {
        OnMiddleMouseUp(pos);
    }
    virtual void right_mouse_down(const Point2 &pos) {
        OnRightMouseDown(pos);
    }
    virtual void right_mouse_drag(const Point2 &pos, const Vector2 &delta) {
        OnRightMouseDrag(pos, delta);
    }
    virtual void right_mouse_up(const Point2 &pos) {
        OnRightMouseUp(pos);
    }
    virtual void key_down(const char *c, int modifiers) {
        OnKeyDown(c, modifiers);
    }
    virtual void key_repeat(const char *c, int modifiers) {
        OnKeyRepeat(c, modifiers);
    }
    virtual void key_up(const char *c, int modifiers) {
        OnKeyUp(c, modifiers);
    }
    virtual void special_key_down(int key, int scancode, int modifiers) {
        OnSpecialKeyDown(key, scancode, modifiers);
    }
    virtual void special_key_repeat(int key, int scancode, int modifiers) {
        OnSpecialKeyRepeat(key, scancode, modifiers);
    }
    virtual void special_key_up(int key, int scancode, int modifiers) {
        OnSpecialKeyUp(key, scancode, modifiers);
    }

    bool graphicsInitialized_;
    int width_;
    int height_;
    const std::string caption_;
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

