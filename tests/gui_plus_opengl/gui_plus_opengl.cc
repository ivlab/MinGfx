
#include "gui_plus_opengl.h"

#include "default_shader.h"
#include "mesh.h"
#include "shader_program.h"
#include "texture2d.h"
#include "unicam.h"
#include <stb_image.h>

#include <iostream>


Draw *qs;
GLuint vbo, vao, vshader, fshader, shaderProgram;
float model[16];
GLuint texID = 0;

Mesh mesh1, mesh2;
ShaderProgram shaderprog1;
Texture2D tex1;

DefaultShader dShader;
DefaultShader::MaterialProperties mat1;

Matrix4 P,V;
Ray pickRay;

UniCam uniCam;

Point2 mpos;


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
        //std::cout << "Update Simulation " << simTime_ << std::endl;
    }
    uniCam.AdvanceAnimation(dt);
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



void GuiPlusOpenGL::OnMouseMove(const Point2 &pos, const Vector2 &delta) {
    //std::cout << "Mouse moved to " << pos << " delta from last frame " << delta << std::endl;
    
    Point2 mouseInNDC = pixels_to_normalized_coordinates(pos);
    Point3 filmplanePt = filmplane2D_to_nearplane3D(uniCam.view_matrix(), P, mouseInNDC);
    
    Matrix4 camMat = uniCam.view_matrix().inverse();
    Point3 eye = camMat.getColumnAsPoint3(3);

    pickRay = Ray(eye, filmplanePt - eye);

    mpos = pos;
}

void GuiPlusOpenGL::OnLeftMouseDown(const Point2 &pos) {
    //std::cout << "Left mouse button down at " << pos << std::endl;
    
    Point2 mouseInNDC = pixels_to_normalized_coordinates(pos);
    float mouseZ = z_value_at_pixel(pos);
    uniCam.OnButtonDown(mouseInNDC, mouseZ);
}

void GuiPlusOpenGL::OnLeftMouseDrag(const Point2 &pos, const Vector2 &delta) {
    //std::cout << "Mouse dragged (left button) to " << pos << " delta from last frame " << delta << std::endl;
    
    Point2 mouseInNDC = pixels_to_normalized_coordinates(pos);
    uniCam.OnDrag(mouseInNDC);
}

void GuiPlusOpenGL::OnLeftMouseUp(const Point2 &pos) {
    //std::cout << "Left mouse button up at " << pos << std::endl;
    
    Point2 mouseInNDC = pixels_to_normalized_coordinates(pos);
    uniCam.OnButtonUp(mouseInNDC);
}


void GuiPlusOpenGL::OnMiddleMouseDown(const Point2 &pos) {
    //std::cout << "Middle mouse button down at " << pos << std::endl;
}

void GuiPlusOpenGL::OnMiddleMouseDrag(const Point2 &pos, const Vector2 &delta) {
    //std::cout << "Mouse dragged (middle button) to " << pos << " delta from last frame " << delta << std::endl;
}

void GuiPlusOpenGL::OnMiddleMouseUp(const Point2 &pos) {
    //std::cout << "Middle mouse button up at " << pos << std::endl;
}


void GuiPlusOpenGL::OnRightMouseDown(const Point2 &pos) {
    //std::cout << "Right mouse button down at " << pos << std::endl;
}

void GuiPlusOpenGL::OnRightMouseDrag(const Point2 &pos, const Vector2 &delta) {
    //std::cout << "Mouse dragged (right button) to " << pos << " delta from last frame " << delta << std::endl;
}

void GuiPlusOpenGL::OnRightMouseUp(const Point2 &pos) {
    //std::cout << "Right mouse button up at " << pos << std::endl;
}



void GuiPlusOpenGL::OnKeyDown(const char *c, int modifiers) {
    //std::cout << "Key DOWN (" << c << ") modifiers=" << modifiers << std::endl;
}

void GuiPlusOpenGL::OnKeyUp(const char *c, int modifiers) {
    //std::cout << "Key UP (" << c << ") modifiers=" << modifiers << std::endl;
}

void GuiPlusOpenGL::OnSpecialKeyDown(int key, int scancode, int modifiers) {
    //std::cout << "Special Key DOWN key=" << key << " scancode=" << scancode << " modifiers=" << modifiers << std::endl;
}

void GuiPlusOpenGL::OnSpecialKeyUp(int key, int scancode, int modifiers) {
    //std::cout << "Special Key UP key=" << key << " scancode=" << scancode << " modifiers=" << modifiers << std::endl;
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


void GuiPlusOpenGL::InitOpenGL() {
    P = Matrix4::perspective(60.0, aspect_ratio(), 0.1, 10.0);
    V = Matrix4::lookAt(Point3(0,0,3), Point3(0,0,0), Vector3(0,1,0));
    uniCam.set_view_matrix(V);

    mesh1.UpdateGPUMemory();
    mesh2.LoadFromOBJ(Platform::findMinGfxDataFile("teapot.obj"));
    
    tex1.InitFromFile(Platform::findMinGfxDataFile("test.png"));
    mat1.surfaceTexture = tex1;
    
    DefaultShader::LightProperties l;
    l.position = Point3(-10, 5, 5);
    l.diffuseIntensity = Color(1,0,0);
    dShader.add_light(l);
}


void GuiPlusOpenGL::DrawUsingOpenGL() {
    
    V = uniCam.view_matrix();
    Matrix4 M = Matrix4::translation(Vector3(-1,0,0)) * Matrix4::scale(Vector3(0.5, 0.5, 0.5));
    
    // Draw several quick shapes
    float col[3] = {0.4, 0.4, 0.8};
    Matrix4 M2 = Matrix4::translation(Vector3(1,1.5,0))*Matrix4::scale(Vector3(0.2, 0.2, 0.2));
    qs->Sphere(M2, V, P, col);

    M2 = Matrix4::translation(Vector3(0,-0.5,0)) * M2;
    qs->Cylinder(M2, V, P, col);

    M2 = Matrix4::translation(Vector3(0,-0.5,0)) * M2;
    qs->Cone(M2, V, P, col);
    
    M2 = Matrix4::translation(Vector3(0,-0.5,0)) * M2;
    qs->Cube(M2, V, P, col);

    M2 = Matrix4::translation(Vector3(0,-0.5,0)) * M2;
    qs->Brush(M2, V, P, col);

    M2 = Matrix4::translation(Vector3(0,-0.5,0)) * M2;
    qs->Square(M2, V, P, col);
    
    M2 = Matrix4::translation(Vector3(0,-0.5,0)) * M2;
    qs->Square(M2, V, P, col, tex1);
    
    
    qs->Arrow(Matrix4(), V, P, col, Point3(0,0,0), Vector3(-1,1.5,0), 0.01);
    
    qs->LineSegment(Matrix4(), V, P, col, Point3(0,0,0), Point3(1, 1.5, 0), 0.01);
    
    qs->Axes(Matrix4(), V, P);
    
    dShader.Draw(Matrix4(), V, P, &mesh1, mat1);
    dShader.Draw(Matrix4(), V, P, &mesh2, DefaultShader::MaterialProperties());
    
    
    float t;
    Point3 p;
    int id;
    if (pickRay.IntersectMesh(mesh1, &t, &p, &id)) {
        M2 = Matrix4::translation(p - Point3::origin()) * Matrix4::scale(Vector3(0.025, 0.025, 0.025));
        qs->Sphere(M2, V, P, Color(1,0,1));
    }
    if (pickRay.IntersectMesh(mesh2, &t, &p, &id)) {
        M2 = Matrix4::translation(p - Point3::origin()) * Matrix4::scale(Vector3(0.025, 0.025, 0.025));
        qs->Sphere(M2, V, P, Color(1,1,0));
    }
    
    uniCam.Draw(P);
    
    //std::cout << mpos << " z = " << z_value_at_pixel(mpos) << std::endl;
}