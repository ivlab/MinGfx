
#include "gui_plus_opengl.h"

#include "mesh.h"
#include "shader_program.h"
#include "texture2d.h"
#include <stb_image.h>

#include <iostream>


Draw *qs;
GLuint vbo, vao, vshader, fshader, shaderProgram;
float model[16];
GLuint texID = 0;

Mesh mesh1, mesh2;
ShaderProgram shaderprog1;
Texture2D tex1;


GuiPlusOpenGL::GuiPlusOpenGL() : GraphicsApp(1024,768, "Circle Simulation") {
    nanogui::FormHelper *gui = new nanogui::FormHelper(screen());
    nanogui::ref<nanogui::Window> window = gui->addWindow(Eigen::Vector2i(10, 10), "Simulation Controls");    
    pauseBtn_ = gui->addButton("Pause", std::bind(&GuiPlusOpenGL::OnPauseBtnPressed, this));
    gui->addButton("Restart", std::bind(&GuiPlusOpenGL::OnRestartBtnPressed, this));

    screen()->performLayout();
     
    simTime_ = 0.0;
    paused_ = false;
    
    qs = new Draw();
    
    
    int i;
    i = mesh1.AddTriangle(Point3(0,0,0), Point3(1,0,0), Point3(1,1,0));
    mesh1.SetNormals(i, Vector3(0,0,1), Vector3(0,0,1), Vector3(0,0,1));
    mesh1.SetTexCoords(i, 0, Point2(0,0), Point2(1,0), Point2(1,1));
    i = mesh1.AddTriangle(Point3(0,0,0), Point3(1,1,0), Point3(0,1,0));
    mesh1.SetNormals(i, Vector3(0,0,1), Vector3(0,0,1), Vector3(0,0,1));
    mesh1.SetTexCoords(i, 0, Point2(0,0), Point2(1,1), Point2(0,1));
    
    //i = mesh1.AppendTriangle(Point3(0,0,0), Point3(0,0,1), Point3(1,1,0));
    //mesh1.SetNormals(i, Vector3(0,0,1), Vector3(0,0,1), Vector3(0,0,1));
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
    
    mesh1.UpdateGPUMemory();
    
    mesh2.LoadFromOBJ(Platform::findMinGfxDataFile("teapot.obj"));
    
    std::string vs =
    "#version 330 \n"
    "layout(location = 0) in vec3 position; \n"
    "layout(location = 1) in vec3 normal; \n"
    "layout(location = 2) in vec4 color; \n"
    "layout(location = 3) in vec2 texcoord;\n"
    "\n"
    "uniform mat4 ModelMatrix; \n"
    "uniform mat4 ViewMatrix; \n"
    "uniform mat4 ProjectionMatrix; \n"
    "uniform mat4 NormalMatrix; \n"
    "\n"
    "out vec3 N; \n"
    "out vec3 v; \n"
    "out vec2 uv; \n"
    "out vec4 col_interp; \n"
    "\n"
    "void main() { \n"
    "   v = (ViewMatrix * ModelMatrix * vec4(position, 1)).xyz; \n"
    "   N = normalize((NormalMatrix * vec4(normal, 0)).xyz); \n"
    "   uv = texcoord.xy; \n"
    "   gl_Position	= ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(position, 1); \n"
    "   col_interp = color; \n"
    "} \n";
    
    std::string fs =
    "#version 330 \n"
    "\n"
    "in vec3 N; \n"
    "in vec3 v; \n"
    "in vec2 uv; \n"
    "in vec4 col_interp; \n"
    "\n"
    "out vec4 fragColor; \n"
    "\n"
    "uniform vec3 LightPosition; \n"
    "uniform vec3 LightIntensityAmbient; \n"
    "uniform vec3 LightIntensityDiffuse; \n"
    "uniform vec3 LightIntensitySpecular; \n"
    "\n"
    "uniform vec3 MatReflectanceAmbient; \n"
    "uniform vec3 MatReflectanceDiffuse; \n"
    "uniform vec3 MatReflectanceSpecular; \n"
    "uniform float MatReflectanceShininess; \n"
    "\n"
    "uniform sampler2D SurfaceTexture; \n"
    "\n"
    "void main() { \n"
    "   vec3 L = normalize(LightPosition - v); \n"
    "   vec3 V = normalize(-v); // eye is at (0,0,0) \n"
    "   vec3 R = normalize(-reflect(L,N));"
    "\n"
    "   vec3 Ia = MatReflectanceAmbient * LightIntensityAmbient; \n"
    "\n"
    "   vec3 Id = clamp(MatReflectanceDiffuse * LightIntensityDiffuse * max(dot(N, L), 0.0), 0.0, 1.0); \n"
    "\n"
    "   vec3 Is = MatReflectanceSpecular * LightIntensitySpecular * pow(max(dot(R, V), 0.0), MatReflectanceShininess); \n"
    "   Is = clamp(Is, 0.0, 1.0);"
    "\n"
    "   fragColor = col_interp; \n"
    "   fragColor *= texture(SurfaceTexture, uv); \n"
    "   fragColor.rgb *= Ia + Id + Is; \n"
    "   //fragColor.rgb = Ia + Id + Is; \n"
    "   //fragColor = vec4(1,1,1,1);\n"
    "} \n";
    
    //shaderprog1.AddVertexShaderFromSource(vs);
    //shaderprog1.AddFragmentShaderFromSource(fs);
    
    shaderprog1.AddVertexShaderFromFile(Platform::findMinGfxShaderFile("phongtex.vert"));
    shaderprog1.AddFragmentShaderFromFile(Platform::findMinGfxShaderFile("phongtex.frag"));
    shaderprog1.LinkProgram();
    
    tex1.InitFromFile(Platform::findMinGfxDataFile("test.png"));
}


void GuiPlusOpenGL::DrawUsingOpenGL() {
    Matrix4 P = Matrix4::perspective(60.0, aspect_ratio(), 0.1, 10.0);
    Matrix4 V = Matrix4::lookAt(Point3(0,0,3), Point3(0,0,0), Vector3(0,1,0));
    Matrix4 M = Matrix4::translation(Vector3(-1,0,0)) * Matrix4::scale(Vector3(0.5, 0.5, 0.5));
    
    // Draw several quick shapes
    float col[3] = {0.4, 0.4, 0.8};
    Matrix4 M2 = Matrix4::translation(Vector3(1,1.5,0))*Matrix4::scale(Vector3(0.2, 0.2, 0.2));
    qs->Sphere(M2.value_ptr(), V.value_ptr(), P.value_ptr(), col);

    M2 = Matrix4::translation(Vector3(0,-0.5,0)) * M2;
    qs->Cylinder(M2.value_ptr(), V.value_ptr(), P.value_ptr(), col);

    M2 = Matrix4::translation(Vector3(0,-0.5,0)) * M2;
    qs->Cone(M2.value_ptr(), V.value_ptr(), P.value_ptr(), col);
    
    M2 = Matrix4::translation(Vector3(0,-0.5,0)) * M2;
    qs->Cube(M2.value_ptr(), V.value_ptr(), P.value_ptr(), col);

    M2 = Matrix4::translation(Vector3(0,-0.5,0)) * M2;
    qs->Brush(M2.value_ptr(), V.value_ptr(), P.value_ptr(), col);

    M2 = Matrix4::translation(Vector3(0,-0.5,0)) * M2;
    qs->Square(M2.value_ptr(), V.value_ptr(), P.value_ptr(), col);
    
    M2 = Matrix4::translation(Vector3(0,-0.5,0)) * M2;
    qs->Square(M2.value_ptr(), V.value_ptr(), P.value_ptr(), col, tex1);
    
    
    qs->LineSegment(Matrix4().value_ptr(), V.value_ptr(), P.value_ptr(),
                        col, Point3(0,0,0), Point3(1, 1.5, 0), 0.01);
    
    
    
    shaderprog1.UseProgram();
    
    M = Matrix4();
    Matrix4 N = (M*V).inverse().transpose();

    shaderprog1.SetUniform("ModelMatrix", M);
    shaderprog1.SetUniform("ViewMatrix", V);
    shaderprog1.SetUniform("ProjectionMatrix", P);
    shaderprog1.SetUniform("NormalMatrix", N);
    shaderprog1.SetUniform("LightPosition", Point3(10,10,10));
    shaderprog1.SetUniform("LightIntensityAmbient", Color(0.2, 0.2, 0.2));
    shaderprog1.SetUniform("LightIntensityDiffuse", Color(0.7, 0.7, 0.7));
    shaderprog1.SetUniform("LightIntensitySpecular", Color(0.6, 0.6, 0.6));
    shaderprog1.SetUniform("MatReflectanceAmbient", Color(0.2, 0.2, 0.2));
    shaderprog1.SetUniform("MatReflectanceDiffuse", Color(0.5, 0.5, 0.5));
    shaderprog1.SetUniform("MatReflectanceSpecular", Color(0.6, 0.6, 0.6));
    shaderprog1.SetUniform("MatReflectanceShininess", 20.0f);
    
    shaderprog1.BindTexture("SurfaceTexture", tex1);
    
    mesh1.Draw();
    mesh2.Draw();
    
    shaderprog1.StopProgram();
    

}