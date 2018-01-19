
#include "graphics_app.h"



namespace MinGfx {



GraphicsApp::GraphicsApp(int width, int height, const std::string &caption) : lastDrawT_(0.0) {
    
    glfwInit();
    
    glfwSetTime(0);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    
    // Create a GLFWwindow object
    window_ = glfwCreateWindow(800, 800, caption.c_str(), nullptr, nullptr);
    if (window_ == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window_);
    glfwSetWindowUserPointer(window_, this);

    
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
    
    glfwGetFramebufferSize(window_, &width, &height);
    glViewport(0, 0, width, height);
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
 }

GraphicsApp::~GraphicsApp() {
}


    
void GraphicsApp::Run() {
    
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
        return true;
    }
    else {
        OnMouseMove(x,y);
    }
    return false;
}

bool GraphicsApp::mouse_button_glfw_cb(int button, int action, int modifiers) {
    if (screen_->mouseButtonCallbackEvent(button, action, modifiers)) {
        return true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT) {
        double x,y;
        glfwGetCursorPos(window_, &x, &y);
        if (action == 1) {
            OnLeftMouseDown(x, y);
        }
        else {
            OnLeftMouseUp(x, y);
        }
        return true;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        double x,y;
        glfwGetCursorPos(window_, &x, &y);
        if (action == 1) {
            OnRightMouseDown(x, y);
        }
        else {
            OnRightMouseUp(x, y);
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
            OnKeyDown(glfwGetKeyName(key, scancode), modifiers);
        }
        else {
            OnKeyUp(glfwGetKeyName(key, scancode), modifiers);
        }
        return true;
    }
    else {
        if (action == GLFW_PRESS) {
            OnSpecialKeyDown(key, scancode, modifiers);
        }
        else {
            OnSpecialKeyUp(key, scancode, modifiers);
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
        // TODO: could add another virtual function to GraphicsApp to
        // respond to this if needed
    }
    return false;
}


bool GraphicsApp::is_key_down(int key) {
    return (glfwGetKey(window_, key) == GLFW_PRESS);
}

bool GraphicsApp::is_left_mouse_down() {
    return (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
}


bool GraphicsApp::is_middle_mouse_down() {
    return (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
}


bool GraphicsApp::is_right_mouse_down() {
    return (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
}

    

float GraphicsApp::aspect_ratio() {
    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    return (float)width/(float)height;
}

    
nanogui::Screen* GraphicsApp::screen() {
    return screen_;
}

GLFWwindow* GraphicsApp::window() {
    return window_;
}


} // end namespace
