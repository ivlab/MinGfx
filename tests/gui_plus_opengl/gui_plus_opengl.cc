
#include "gui_plus_opengl.h"
#include <iostream>


QuickShapes *qs;
GLuint vbo, vao, vshader, fshader, shaderProgram;
float model[16];

GuiPlusOpenGL::GuiPlusOpenGL() : GraphicsApp(1024,768, "Circle Simulation") {
    nanogui::FormHelper *gui = new nanogui::FormHelper(screen());
    nanogui::ref<nanogui::Window> window = gui->addWindow(Eigen::Vector2i(10, 10), "Simulation Controls");    
    pauseBtn_ = gui->addButton("Pause", std::bind(&GuiPlusOpenGL::OnPauseBtnPressed, this));
    gui->addButton("Restart", std::bind(&GuiPlusOpenGL::OnRestartBtnPressed, this));

    screen()->performLayout();
     
    simTime_ = 0.0;
    paused_ = false;
    
    qs = new QuickShapes();
}


GuiPlusOpenGL::~GuiPlusOpenGL() {
    
}


void GuiPlusOpenGL::UpdateSimulation(double dt) {
    if (!paused_) {
        simTime_ += dt;
        std::cout << "Update Simulation " << simTime_ << std::endl;
    }
}


void GuiPlusOpenGL::OnRestartBtnPressed() {
    simTime_ = 0.0;
}

void GuiPlusOpenGL::OnPauseBtnPressed() {
    paused_ = !paused_;
    if (paused_) {
        pauseBtn_->setCaption("Play");
    }
    else {
        pauseBtn_->setCaption("Pause");
    }
}


void GuiPlusOpenGL::OnMouseMove(int x, int y) {
    std::cout << "Mouse moved to (" << x << ", " << y << ")" << std::endl; 
}

void GuiPlusOpenGL::OnLeftMouseDown(int x, int y) {
    std::cout << "Left mouse button DOWN (" << x << ", " << y << ")" << std::endl; 
}

void GuiPlusOpenGL::OnLeftMouseUp(int x, int y) {
    std::cout << "Left mouse button UP (" << x << ", " << y << ")" << std::endl; 
}

void GuiPlusOpenGL::OnRightMouseDown(int x, int y) {
    std::cout << "Right mouse button DOWN (" << x << ", " << y << ")" << std::endl; 
}

void GuiPlusOpenGL::OnRightMouseUp(int x, int y) {
    std::cout << "Right mouse button UP (" << x << ", " << y << ")" << std::endl; 
}

void GuiPlusOpenGL::OnKeyDown(const char *c, int modifiers) {
    std::cout << "Key DOWN (" << c << ") modifiers=" << modifiers << std::endl; 
}

void GuiPlusOpenGL::OnKeyUp(const char *c, int modifiers) {
    std::cout << "Key UP (" << c << ") modifiers=" << modifiers << std::endl; 
}

void GuiPlusOpenGL::OnSpecialKeyDown(int key, int scancode, int modifiers) {
    std::cout << "Special Key DOWN key=" << key << " scancode=" << scancode << " modifiers=" << modifiers << std::endl; 
}

void GuiPlusOpenGL::OnSpecialKeyUp(int key, int scancode, int modifiers) {
    std::cout << "Special Key UP key=" << key << " scancode=" << scancode << " modifiers=" << modifiers << std::endl; 
}


void GuiPlusOpenGL::DrawUsingNanoVG(NVGcontext *ctx) {
    // example of drawing some circles
    
    nvgBeginPath(ctx);
    nvgCircle(ctx, 512+50.0*cos(simTime_), 350+200.0*sin(simTime_), 50);
    nvgFillColor(ctx, nvgRGBA(100,100,255,200));
    nvgFill(ctx);
    nvgStrokeColor(ctx, nvgRGBA(0,0,0,255));
    nvgStroke(ctx);

    nvgBeginPath(ctx);
    nvgCircle(ctx, 512+200.0*cos(simTime_), 350+50.0*sin(simTime_), 50);
    nvgFillColor(ctx, nvgRGBA(255,100,100,200));
    nvgFill(ctx);
    nvgStrokeColor(ctx, nvgRGBA(0,0,0,255));
    nvgStroke(ctx);
        
}



/// Compiles shader
GLuint compileShader(const std::string& shaderText, GLuint shaderType) {
    const char* source = shaderText.c_str();
    int length = shaderText.size();
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, &length);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE) {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetShaderInfoLog(shader, length, &length, &log[0]);
        std::cerr << &log[0];
    }
    
    return shader;
}

/// links shader program
void linkShaderProgram(GLuint shaderProgram) {
    glLinkProgram(shaderProgram);
    GLint status;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if(status == GL_FALSE) {
        GLint length;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetProgramInfoLog(shaderProgram, length, &length, &log[0]);
        std::cerr << "Error compiling program: " << &log[0] << std::endl;
    }
}


void GuiPlusOpenGL::InitOpenGL() {
    #ifdef _WIN32
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        if (GLEW_OK != err) {
            std::cerr << "Error initializing GLEW." << std::endl;
        }
    #endif
    
    // Init GL
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0, 0, 0, 1);
    
    // Create VBO
    GLfloat vertices[]  = { 1.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,  -1.0f,-1.0f, 1.0f,      // v0-v1-v2 (front)
        -1.0f,-1.0f, 1.0f,   1.0f,-1.0f, 1.0f,   1.0f, 1.0f, 1.0f,      // v2-v3-v0
        
        1.0f, 1.0f, 1.0f,   1.0f,-1.0f, 1.0f,   1.0f,-1.0f,-1.0f,      // v0-v3-v4 (right)
        1.0f,-1.0f,-1.0f,   1.0f, 1.0f,-1.0f,   1.0f, 1.0f, 1.0f,      // v4-v5-v0
        
        1.0f, 1.0f, 1.0f,   1.0f, 1.0f,-1.0f,  -1.0f, 1.0f,-1.0f,      // v0-v5-v6 (top)
        -1.0f, 1.0f,-1.0f,  -1.0f, 1.0f, 1.0f,   1.0f, 1.0f, 1.0f,      // v6-v1-v0
        
        -1.0f, 1.0f, 1.0f,  -1.0f, 1.0f,-1.0f,  -1.0f,-1.0f,-1.0f,      // v1-v6-v7 (left)
        -1.0f,-1.0f,-1.0f,  -1.0f,-1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,      // v7-v2-v1.0
        
        -1.0f,-1.0f,-1.0f,   1.0f,-1.0f,-1.0f,   1.0f,-1.0f, 1.0f,      // v7-v4-v3 (bottom)
        1.0f,-1.0f, 1.0f,  -1.0f,-1.0f, 1.0f,  -1.0f,-1.0f,-1.0f,      // v3-v2-v7
        
        1.0f,-1.0f,-1.0f,  -1.0f,-1.0f,-1.0f,  -1.0f, 1.0f,-1.0f,      // v4-v7-v6 (back)
        -1.0f, 1.0f,-1.0f,   1.0f, 1.0f,-1.0f,   1.0f,-1.0f,-1.0f };    // v6-v5-v4
    
    // normal array
    GLfloat normals[]   = { 0, 0, 1,   0, 0, 1,   0, 0, 1,      // v0-v1-v2 (front)
        0, 0, 1,   0, 0, 1,   0, 0, 1,      // v2-v3-v0
        
        1, 0, 0,   1, 0, 0,   1, 0, 0,      // v0-v3-v4 (right)
        1, 0, 0,   1, 0, 0,   1, 0, 0,      // v4-v5-v0
        
        0, 1, 0,   0, 1, 0,   0, 1, 0,      // v0-v5-v6 (top)
        0, 1, 0,   0, 1, 0,   0, 1, 0,      // v6-v1-v0
        
        -1, 0, 0,  -1, 0, 0,  -1, 0, 0,      // v1-v6-v7 (left)
        -1, 0, 0,  -1, 0, 0,  -1, 0, 0,      // v7-v2-v1
        
        0,-1, 0,   0,-1, 0,   0,-1, 0,      // v7-v4-v3 (bottom)
        0,-1, 0,   0,-1, 0,   0,-1, 0,      // v3-v2-v7
        
        0, 0,-1,   0, 0,-1,   0, 0,-1,      // v4-v7-v6 (back)
        0, 0,-1,   0, 0,-1,   0, 0,-1 };    // v6-v5-v4
    
    // color array
    GLfloat colors[]    = { 1, 1, 1,   1, 1, 0,   1, 0, 0,      // v0-v1-v2 (front)
        1, 0, 0,   1, 0, 1,   1, 1, 1,      // v2-v3-v0
        
        1, 1, 1,   1, 0, 1,   0, 0, 1,      // v0-v3-v4 (right)
        0, 0, 1,   0, 1, 1,   1, 1, 1,      // v4-v5-v0
        
        1, 1, 1,   0, 1, 1,   0, 1, 0,      // v0-v5-v6 (top)
        0, 1, 0,   1, 1, 0,   1, 1, 1,      // v6-v1-v0
        
        1, 1, 0,   0, 1, 0,   0, 0, 0,      // v1-v6-v7 (left)
        0, 0, 0,   1, 0, 0,   1, 1, 0,      // v7-v2-v1
        
        0, 0, 0,   0, 0, 1,   1, 0, 1,      // v7-v4-v3 (bottom)
        1, 0, 1,   1, 0, 0,   0, 0, 0,      // v3-v2-v7
        
        0, 0, 1,   0, 0, 0,   0, 1, 0,      // v4-v7-v6 (back)
        0, 1, 0,   0, 1, 1,   0, 0, 1 };    // v6-v5-v4
    
    // Allocate space and send Vertex Data
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)+sizeof(normals)+sizeof(colors), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(normals), normals);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices)+sizeof(normals), sizeof(colors), colors);
    
    // Create vao
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (char*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (char*)0 + sizeof(vertices));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (char*)0 + sizeof(vertices) + sizeof(normals));
    
    // Create shader
    std::string vertexShader =
    "#version 330 \n"
    "layout(location = 0) in vec3 position; "
    "layout(location = 1) in vec3 normal; "
    "layout(location = 2) in vec3 color; "
    ""
    "uniform mat4 ProjectionMatrix; "
    "uniform mat4 ViewMatrix; "
    "uniform mat4 ModelMatrix; "
    "uniform mat4 NormalMatrix; "
    ""
    "out vec3 col;"
    ""
    "void main() { "
    "	gl_Position = ProjectionMatrix*ViewMatrix*ModelMatrix*vec4(position, 1.0); "
    "	col = color;"
    "}";
    vshader = compileShader(vertexShader, GL_VERTEX_SHADER);
    
    std::string fragmentShader =
    "#version 330 \n"
    "in vec3 col;"
    "out vec4 colorOut;"
    ""
    "void main() { "
    "	colorOut = vec4(col, 1.0); "
    "}";
    fshader = compileShader(fragmentShader, GL_FRAGMENT_SHADER);
    
    // Create shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vshader);
    glAttachShader(shaderProgram, fshader);
    linkShaderProgram(shaderProgram);
}


void GuiPlusOpenGL::DrawUsingOpenGL() {
    // clear screen
    glClearColor(0, 0, 0, 1);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    glFrontFace(GL_CCW);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    glDisable(GL_BLEND);
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);


    
    Matrix4 P = Matrix4::perspective(60.0, aspect_ratio(), 0.1, 10.0);
    Matrix4 V = Matrix4::lookAt(Point3(1,1,3), Point3(0,0,0), Vector3(0,1,0));
    Matrix4 M = Matrix4::translation(Vector3(-1,0,0)) * Matrix4::scale(Vector3(0.5, 0.5, 0.5));
    
    // Set shader parameters
    glUseProgram(shaderProgram);
    GLint loc = glGetUniformLocation(shaderProgram, "ProjectionMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, P.value_ptr());
    loc = glGetUniformLocation(shaderProgram, "ViewMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, V.value_ptr());
    loc = glGetUniformLocation(shaderProgram, "ModelMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, M.value_ptr());

    // Draw color cube
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Draw several quick shapes
    float col[3] = {0.4, 0.4, 0.8};
    Matrix4 M2 = Matrix4::translation(Vector3(1,1.5,0))*Matrix4::scale(Vector3(0.2, 0.2, 0.2));
    qs->drawCube(M2.value_ptr(), V.value_ptr(), P.value_ptr(), col);

    M2 = Matrix4::translation(Vector3(0,-0.5,0)) * M2;
    qs->drawCylinder(M2.value_ptr(), V.value_ptr(), P.value_ptr(), col);

    M2 = Matrix4::translation(Vector3(0,-0.5,0)) * M2;
    qs->drawSphere(M2.value_ptr(), V.value_ptr(), P.value_ptr(), col);

    M2 = Matrix4::translation(Vector3(0,-0.5,0)) * M2;
    qs->drawBrush(M2.value_ptr(), V.value_ptr(), P.value_ptr(), col);

    
    // reset program
    glUseProgram(0);
}

