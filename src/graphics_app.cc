/*
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 See corresponding header file for details.
 */

#include "graphics_app.h"


namespace mingfx {



GraphicsApp::GraphicsApp(int width, int height, const std::string &caption) :
    graphicsInitialized_(false), lastDrawT_(0.0), leftDown_(false), middleDown_(false), rightDown_(false), screen_(NULL), window_(NULL)
{
    settings_.window_width = width;
    settings_.window_height = height;
    settings_.window_caption = caption;
}

GraphicsApp::GraphicsApp(const GraphicsSettings& settings) :
    graphicsInitialized_(false), lastDrawT_(0.0), leftDown_(false), middleDown_(false), rightDown_(false), screen_(NULL), window_(NULL)
{
    settings_ = settings;
}

GraphicsApp::GraphicsApp() :
    graphicsInitialized_(false), lastDrawT_(0.0), leftDown_(false), middleDown_(false), rightDown_(false), screen_(NULL), window_(NULL)
{}

GraphicsApp::~GraphicsApp() {
}

void GraphicsApp::InitGraphicsContext(const GraphicsSettings& settings) {
    settings_ = settings;
    InitGraphicsContext();
}

void GraphicsApp::InitGraphicsContext() {

    glfwInit();
    
    glfwSetTime(0);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, settings_.gl_version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, settings_.gl_version_minor);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    glfwWindowHint(GLFW_SAMPLES, settings_.multi_samples);
    glfwWindowHint(GLFW_RED_BITS, settings_.red_bits);
    glfwWindowHint(GLFW_GREEN_BITS, settings_.green_bits);
    glfwWindowHint(GLFW_BLUE_BITS, settings_.blue_bits);
    glfwWindowHint(GLFW_ALPHA_BITS, settings_.alpha_bits);
    glfwWindowHint(GLFW_STENCIL_BITS, settings_.stencil_bits);
    glfwWindowHint(GLFW_DEPTH_BITS, settings_.depth_bits);

    glfwWindowHint(GLFW_RESIZABLE, settings_.window_resizable);
    glfwWindowHint(GLFW_DECORATED, settings_.window_decorated);

    
    
    // on OSX, glfwCreateWindow bombs if we pass caption_.c_str() in for the 3rd
    // parameter perhaps because it's const.  so, copying to a tmp char array here.
    char *title = new char[settings_.window_caption.size() + 1];
    for (int i = 0; i < settings_.window_caption.size(); i++) {
        title[i] = settings_.window_caption[i];
    }
    title[settings_.window_caption.size()] = '\0';
    
    // Create a GLFWwindow object
    window_ = glfwCreateWindow(settings_.window_width, settings_.window_height, title, NULL, NULL);
    if (window_ == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window_);
    glfwSetWindowUserPointer(window_, this);

    glfwSetWindowPos(window_, settings_.window_x_pos, settings_.window_y_pos);

    // cleanup tmp title hack
    delete [] title;
    
    
#if defined(NANOGUI_GLAD)
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
       throw std::runtime_error("Could not initialize GLAD!");
    glGetError(); // pull and ignore unhandled errors like GL_INVALID_ENUM
#endif
    
    glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Create a nanogui screen and pass the glfw pointer to initialize
    screen_ = new nanogui::Screen();
    screen_->initialize(window_, true);
    
    glfwGetFramebufferSize(window_, &settings_.window_width, &settings_.window_height);
    glViewport(0, 0, settings_.window_width, settings_.window_height);
    glfwSwapInterval(0);
    glfwSwapBuffers(window_);
    
    screen_->setVisible(true);
    screen_->performLayout();

    glfwSetCursorPosCallback(window_,
        [](GLFWwindow *window, double x, double y) {
            GraphicsApp *app = (GraphicsApp*)glfwGetWindowUserPointer(window);
             app->cursor_pos_glfw_cb(x, y);
         }
    );

    glfwSetMouseButtonCallback(window_,
        [](GLFWwindow *window, int button, int action, int modifiers) {
            GraphicsApp *app = (GraphicsApp*)glfwGetWindowUserPointer(window);
            app->mouse_button_glfw_cb(button, action, modifiers);
        }
    );

    glfwSetKeyCallback(window_,
        [](GLFWwindow *window, int key, int scancode, int action, int mods) {
            GraphicsApp *app = (GraphicsApp*)glfwGetWindowUserPointer(window);
             app->key_glfw_cb(key, scancode, action, mods);
        }
    );

    glfwSetCharCallback(window_,
        [](GLFWwindow *window, unsigned int codepoint) {
            GraphicsApp *app = (GraphicsApp*)glfwGetWindowUserPointer(window);
            app->char_glfw_cb(codepoint);
        }
    );

    glfwSetDropCallback(window_,
        [](GLFWwindow *window, int count, const char **filenames) {
            GraphicsApp *app = (GraphicsApp*)glfwGetWindowUserPointer(window);
            app->drop_glfw_cb(count, filenames);
        }
    );

    glfwSetScrollCallback(window_,
        [](GLFWwindow *window, double x, double y) {
            GraphicsApp *app = (GraphicsApp*)glfwGetWindowUserPointer(window);
            app->scroll_glfw_cb(x, y);
        }
    );

    glfwSetFramebufferSizeCallback(window_,
        [](GLFWwindow *window, int width, int height) {
            GraphicsApp *app = (GraphicsApp*)glfwGetWindowUserPointer(window);
            app->resize_glfw_cb(width, height);
        }
    );

    graphicsInitialized_ = true;
 }


    
void GraphicsApp::Run() {

    if (!graphicsInitialized_) {
        InitGraphicsContext();        
    }

    InitNanoGUI();

    InitOpenGL();
    
    // Main program loop
    glfwSetTime(0.0);
    while (!glfwWindowShouldClose(window_)) {

        // Poll for new user input events and call callbacks
        glfwPollEvents();

        // Update the simulation, i.e., perform all non-graphics updates that
        // should happen each frame.
        double now = glfwGetTime();
        UpdateSimulation(now-lastDrawT_);
        lastDrawT_ = now;
        
        // Clear is handled in this mainloop so that drawing works even for
        // users who do not want to fill in DrawUsingOpenGL()
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // NanoGUI sets these to something other than the OpenGL defaults, which
        // screws up most OpenGL programs, so we need to reset them each frame here.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        
        // Users may fill this in to do raw OpenGL rendering
        DrawUsingOpenGL();

        // This renders the nanogui widgets created on screen_
        screen_->drawContents();
        screen_->drawWidgets();
        
        // Users may fill this in to do additional 2D rendering with the NanoVG library
        DrawUsingNanoVG(screen_->nvgContext());
        
        glfwSwapBuffers(window_);
    }
    
    glfwTerminate();
}
    


bool GraphicsApp::cursor_pos_glfw_cb(double x, double y) {
    
    if (screen_->cursorPosCallbackEvent(x,y)) {
        // event was handled by nanogui
        lastMouse_ = Point2((float)x, (float)y);
        return true;
    }
    else {
        Point2 cur((float)x, (float)y);
        Vector2 delta = cur - lastMouse_;

        // if no buttons are down, generate a mouse move event
        if (!leftDown_ && !middleDown_ && !rightDown_) {
            mouse_move(cur, delta);
        }
        
        // if a button is down, generate a corresponding mouse drag event
        if (leftDown_) {
            left_mouse_drag(cur, delta);
        }
        if (middleDown_) {
            middle_mouse_drag(cur, delta);
        }
        if (rightDown_) {
            right_mouse_drag(cur, delta);
        }
        
        lastMouse_ = cur;
        return false;
    }
}

bool GraphicsApp::mouse_button_glfw_cb(int button, int action, int modifiers) {
    if (screen_->mouseButtonCallbackEvent(button, action, modifiers)) {
        return true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT) {
        double x,y;
        glfwGetCursorPos(window_, &x, &y);
        if (action == 1) {
            left_mouse_down(Point2((float)x, (float)y));
            leftDown_ = true;
        }
        else {
            left_mouse_up(Point2((float)x, (float)y));
            leftDown_ = false;
        }
        return true;
    }
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        double x,y;
        glfwGetCursorPos(window_, &x, &y);
        if (action == 1) {
            middle_mouse_down(Point2((float)x, (float)y));
            middleDown_ = true;
        }
        else {
            middle_mouse_up(Point2((float)x, (float)y));
            middleDown_ = false;
        }
        return true;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        double x,y;
        glfwGetCursorPos(window_, &x, &y);
        if (action == 1) {
            right_mouse_down(Point2((float)x, (float)y));
            rightDown_ = true;
        }
        else {
            right_mouse_up(Point2((float)x, (float)y));
            rightDown_ = false;
        }
        return true;
    }
    return false;
}


bool GraphicsApp::key_glfw_cb(int key, int scancode, int action, int modifiers) {
    if (screen_->keyCallbackEvent(key, scancode, action, modifiers)) {
        return true;
    }
    else {
    if (glfwGetKeyName(key, scancode) != NULL) {
        if (action == GLFW_PRESS) {
            key_down(glfwGetKeyName(key, scancode), modifiers);
        }
        else if (action == GLFW_REPEAT) {
            key_repeat(glfwGetKeyName(key, scancode), modifiers);
        }
        else {
            key_up(glfwGetKeyName(key, scancode), modifiers);
        }
        return true;
    }
    else {
        if (action == GLFW_PRESS) {
            special_key_down(key, scancode, modifiers);
        }
        else if (action == GLFW_REPEAT) {
            special_key_repeat(key, scancode, modifiers);
        }
        else {
            special_key_up(key, scancode, modifiers);
        }
        return true;
    }
    }

    return false;
}


bool GraphicsApp::char_glfw_cb(unsigned int codepoint) {
    if (screen_->charCallbackEvent(codepoint)) {
        return true;
    }
    else {
        // TODO: could add another virtual function to GraphicsApp to
        // respond to this if needed
    }
    return false;
}


bool GraphicsApp::drop_glfw_cb(int count, const char **filenames) {
    if (screen_->dropCallbackEvent(count, filenames)) {
        return true;
    }
    else {
        // TODO: could add another virtual function to GraphicsApp to
        // respond to this if needed
    }
    return false;
}


bool GraphicsApp::scroll_glfw_cb(double x, double y) {
    if (screen_->scrollCallbackEvent(x,y)) {
        return true;
    }
    else {
        // TODO: could add another virtual function to GraphicsApp to
        // respond to this if needed
    }
    return false;
}


bool GraphicsApp::resize_glfw_cb(int width, int height) {
    if (screen_->resizeCallbackEvent(width, height)) {
        return true;
    }
    else {
        // the width and height reported here are the new framebuffer size
        // we will query/save/report the new window size instead
        settings_.window_width = window_width();
        settings_.window_height = window_height();
        OnWindowResize(settings_.window_width, settings_.window_height);
    }
    return false;
}


bool GraphicsApp::IsKeyDown(int key) {
    return (glfwGetKey(window_, key) == GLFW_PRESS);
}

bool GraphicsApp::IsLeftMouseDown() {
    return (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
}


bool GraphicsApp::IsMiddleMouseDown() {
    return (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
}


bool GraphicsApp::IsRightMouseDown() {
    return (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
}

    

float GraphicsApp::aspect_ratio() {
    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    return (float)width/(float)height;
}
    
int GraphicsApp::window_width() {
    int width, height;
    glfwGetWindowSize(window_, &width, &height);
    return width;
}

int GraphicsApp::framebuffer_width() {
    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    return width;
}

int GraphicsApp::window_height() {
    int width, height;
    glfwGetWindowSize(window_, &width, &height);
    return height;
}

int GraphicsApp::framebuffer_height() {
    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    return height;
}
    
Point2 GraphicsApp::PixelsToNormalizedDeviceCoords(const Point2 &pointInPixels) {
    float x = (pointInPixels[0] / window_width()) * 2.0f - 1.0f;
    float y = (1.0f - (pointInPixels[1] / window_height())) * 2.0f - 1.0f;
    return Point2(x,y);
}

Point2 GraphicsApp::NormalizedDeviceCoordsToPixels(const Point2 &pointInNDC) {
    float x = 0.5f * (pointInNDC[0] + 1.0f) * window_width();
    float y = (1.0f - (0.5f * (pointInNDC[1] + 1.0f))) * window_height();
    return Point2(x,y);
}

Vector2 GraphicsApp::PixelsToNormalizedDeviceCoords(const Vector2 &vectorInPixels) {
    float x = (2.0f/window_width()) * vectorInPixels[0];
    float y = (-2.0f/window_height()) * vectorInPixels[1];
    return Vector2(x,y);
}

Vector2 GraphicsApp::NormalizedDeviceCoordsToPixels(const Vector2 &vectorInNDC) {
    float x = (window_width()/2.0f) * vectorInNDC[0];
    float y = (-window_height()/2.0f) * vectorInNDC[1];
    return Vector2(x,y);
}

float GraphicsApp::ReadZValueAtPixel(const Point2 &pointInPixels, unsigned int whichBuffer) {
    // scale screen points to framebuffer size, since they are not the same on retina displays
    float x01 = pointInPixels[0] / window_width();
    float y01 = pointInPixels[1] / window_height();
    y01 = 1.0f - y01;
    
    float x = x01 * (float)framebuffer_width();
    float y = y01 * (float)framebuffer_height();

    float z;
    glReadPixels((int)x, (int)y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
    return z;
}
    
    
nanogui::Screen* GraphicsApp::screen() {
    return screen_;
}

GLFWwindow* GraphicsApp::window() {
    return window_;
}
    
    
void GraphicsApp::ResizeWindow(int new_width, int new_height) {
    glfwSetWindowSize(window_, new_width, new_height);
    settings_.window_width = new_width;
    settings_.window_height = new_height;
    OnWindowResize(new_width, new_height);
}

    
    


} // end namespace
