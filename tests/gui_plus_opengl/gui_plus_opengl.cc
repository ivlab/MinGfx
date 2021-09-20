/*
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 See corresponding header file for details.
 */


#define _USE_MATH_DEFINES
#include "gui_plus_opengl.h"
#include "default_shader.h"
#include "mesh.h"
#include "shader_program.h"
#include "texture2d.h"
#include "text_shader.h"
#include "unicam.h"


#pragma warning (push)
#pragma warning (disable : 6001)
#pragma warning (disable : 6011)
#pragma warning (disable : 6262)
#pragma warning (disable : 6385)
#pragma warning (disable : 6387)
#pragma warning (disable : 26450)
#pragma warning (disable : 26451)
#pragma warning (disable : 26453)
#pragma warning (disable : 26495)
#pragma warning (disable : 26812)
#include <stb_image.h>
#pragma warning (pop)

#include <iostream>


QuickShapes *qs;
Ray pickRay;


GLuint vbo, vao, vshader, fshader, shaderProgram;
float model[16];
GLuint texID = 0;

Mesh mesh1, mesh2;
ShaderProgram shaderprog1;
Texture2D tex1;

DefaultShader dShader;
DefaultShader::MaterialProperties mat1;

Matrix4 P,V;

UniCam uniCam;

Point2 mpos;


TextShader ts;



GuiPlusOpenGL::GuiPlusOpenGL() : GraphicsApp(1024,768, "Circle Simulation") {

    InitGraphicsContext();
    
    simTime_ = 0.0;
    paused_ = false;
    pauseBtn_ = NULL;
    
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

void GuiPlusOpenGL::InitNanoGUI() {
    nanogui::FormHelper *gui = new nanogui::FormHelper(screen());
    nanogui::ref<nanogui::Window> window = gui->addWindow(Eigen::Vector2i(10, 10), "Simulation Controls");    
    pauseBtn_ = gui->addButton("Pause", std::bind(&GuiPlusOpenGL::OnPauseBtnPressed, this));
    gui->addButton("Restart", std::bind(&GuiPlusOpenGL::OnRestartBtnPressed, this));

    screen()->performLayout();
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
    
    Point2 mouseInNDC = PixelsToNormalizedDeviceCoords(pos);
    Point3 filmplanePt = GfxMath::ScreenToNearPlane(uniCam.view_matrix(), P, mouseInNDC);
    
    Matrix4 camMat = uniCam.view_matrix().Inverse();
    Point3 eye = camMat.ColumnToPoint3(3);

    pickRay = Ray(eye, filmplanePt - eye);
    
    mpos = pos;
}

void GuiPlusOpenGL::OnLeftMouseDown(const Point2 &pos) {
    //std::cout << "Left mouse button down at " << pos << std::endl;
    
    Point2 mouseInNDC = PixelsToNormalizedDeviceCoords(pos);
    float mouseZ = ReadZValueAtPixel(pos);
    uniCam.OnButtonDown(mouseInNDC, mouseZ);
    
}

void GuiPlusOpenGL::OnLeftMouseDrag(const Point2 &pos, const Vector2 &delta) {
    //std::cout << "Mouse dragged (left button) to " << pos << " delta from last frame " << delta << std::endl;
    
    Point2 mouseInNDC = PixelsToNormalizedDeviceCoords(pos);
    uniCam.OnDrag(mouseInNDC);
}

void GuiPlusOpenGL::OnLeftMouseUp(const Point2 &pos) {
    //std::cout << "Left mouse button up at " << pos << std::endl;
    
    Point2 mouseInNDC = PixelsToNormalizedDeviceCoords(pos);
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
    Point2 mouseInNDC = PixelsToNormalizedDeviceCoords(pos);
    Point3 filmplanePt = GfxMath::ScreenToNearPlane(uniCam.view_matrix(), P, mouseInNDC);
    
    Matrix4 camMat = uniCam.view_matrix().Inverse();
    Point3 eye = camMat.ColumnToPoint3(3);
    
    pickRay = Ray(eye, filmplanePt - eye);
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
    nvgCircle(ctx, 512.0f+50.0f*std::cosf((float)simTime_), 350.0f+200.0f*std::sinf((float)simTime_), 50.0f);
    nvgFillColor(ctx, nvgRGBA(100,100,255,200));
    nvgFill(ctx);
    nvgStrokeColor(ctx, nvgRGBA(0,0,0,255));
    nvgStroke(ctx);

    nvgBeginPath(ctx);
    nvgCircle(ctx, 512.0f+200.0f*std::cosf((float)simTime_), 350.0f+50.0f*std::sinf((float)simTime_), 50.0f);
    nvgFillColor(ctx, nvgRGBA(255,100,100,200));
    nvgFill(ctx);
    nvgStrokeColor(ctx, nvgRGBA(0,0,0,255));
    nvgStroke(ctx);
        
}


void GuiPlusOpenGL::InitOpenGL() {
    std::string fname = Platform::FindMinGfxDataFile("Futura_Medium_BT.ttf");
    ts.Init(fname, 42);
        
    qs = new QuickShapes();

    P = Matrix4::Perspective(60.0f, aspect_ratio(), 0.1f, 10.0f);
    V = Matrix4::LookAt(Point3(0,0,3), Point3(0,0,0), Vector3(0,1,0));
    uniCam.set_view_matrix(V);

    mesh1.UpdateGPUMemory();
    
    mesh2.LoadFromOBJ(Platform::FindMinGfxDataFile("teapot.obj"));
    std::vector<Matrix4> xforms;
    for (float x=-4.0; x <= 4.0; x += 1.0) {
        //xforms.push_back(Matrix4());
        xforms.push_back(Matrix4::Translation(Vector3(x,0,0)));
    }
    mesh2.SetInstanceTransforms(xforms);
    
    
    tex1.InitFromFile(Platform::FindMinGfxDataFile("test.png"));
    mat1.surface_texture = tex1;
    
    DefaultShader::LightProperties l;
    l.position = Point3(-10, 5, 5);
    l.diffuse_intensity = Color(1,0,0);
    dShader.AddLight(l);
}


void GuiPlusOpenGL::DrawUsingOpenGL() {
    
    V = uniCam.view_matrix();
    Matrix4 M = Matrix4::Translation(Vector3(-1,0,0)) * Matrix4::Scale(Vector3(0.5, 0.5, 0.5));

    // Draw several quick shapes
    float col[3] = {0.4f, 0.4f, 0.8f};
    Matrix4 M2 = Matrix4::Translation(Vector3(1.0f, 1.5f, 0.0f))*Matrix4::Scale(Vector3(0.2f, 0.2f, 0.2f));
    qs->DrawSphere(M2, V, P, col);

    M2 = Matrix4::Translation(Vector3(0,-0.5,0)) * M2;
    qs->DrawCylinder(M2, V, P, col);

    M2 = Matrix4::Translation(Vector3(0,-0.5,0)) * M2;
    qs->DrawCone(M2, V, P, col);
    
    M2 = Matrix4::Translation(Vector3(0,-0.5,0)) * M2;
    qs->DrawCube(M2, V, P, col);

    M2 = Matrix4::Translation(Vector3(0,-0.5,0)) * M2;
    qs->DrawBrush(M2, V, P, col);

    M2 = Matrix4::Translation(Vector3(0,-0.5,0)) * M2;
    qs->DrawSquare(M2, V, P, col);
    
    M2 = Matrix4::Translation(Vector3(0,-0.5,0)) * M2;
    qs->DrawSquare(M2, V, P, col, tex1);
    
    
    qs->DrawArrow(Matrix4(), V, P, col, Point3(0,0,0), Vector3(-1,1.5,0), 0.01f);
    
    qs->DrawLineSegment(Matrix4(), V, P, col, Point3(0,0,0), Point3(1, 1.5, 0), 0.01f);
    
    qs->DrawAxes(Matrix4(), V, P);
    
    dShader.Draw(Matrix4(), V, P, &mesh1, mat1);
    dShader.Draw(Matrix4(), V, P, &mesh2, DefaultShader::MaterialProperties());
    
    
    float t;
    Point3 p;
    int id;
    if (pickRay.IntersectMesh(mesh1, &t, &p, &id)) {
        M2 = Matrix4::Translation(p - Point3::Origin()) * Matrix4::Scale(Vector3(0.025f, 0.025f, 0.025f));
        qs->DrawSphere(M2, V, P, Color(1,0,1));
    }
    if (pickRay.IntersectMesh(mesh2, &t, &p, &id)) {
        M2 = Matrix4::Translation(p - Point3::Origin()) * Matrix4::Scale(Vector3(0.025f, 0.025f, 0.025f));
        qs->DrawSphere(M2, V, P, Color(1,1,0));
    }
    
    uniCam.Draw(P);
    
    
    Matrix4 M3 = Matrix4::Translation(Vector3(-1,0,0));
    TextShader::TextFormat f;
    
    
    f.color = Color(1,1,0);
    f.h_align = TextShader::HorizAlign::HORIZ_ALIGN_CENTER;
    f.v_align = TextShader::VertAlign::VERT_ALIGN_TOP;
    ts.Draw3D(M3, V, P, "TOPgg", f, true);
    
    f.color = Color(0,1,1);
    f.h_align = TextShader::HorizAlign::HORIZ_ALIGN_CENTER;
    f.v_align = TextShader::VertAlign::VERT_ALIGN_CENTER;
    ts.Draw3D(M3, V, P, "V_CENTERyy", f, true);
    
    f.color = Color(1,0,1);
    f.h_align = TextShader::HorizAlign::HORIZ_ALIGN_CENTER;
    f.v_align = TextShader::VertAlign::VERT_ALIGN_BOTTOM;
    ts.Draw3D(M3, V, P, "BOTTOMgg", f, true);
    
    f.color = Color(1,1,1);
    f.h_align = TextShader::HorizAlign::HORIZ_ALIGN_CENTER;
    f.v_align = TextShader::VertAlign::VERT_ALIGN_BASELINE;
    ts.Draw3D(M3, V, P, "Hello good buddy", f, true);
    
    qs->DrawLineSegment(M3, V, P, col, Point3(0,0,0), Point3(1, 0, 0), 0.01f);

    M3 = M3 * Matrix4::Translation(Vector3(0,0.5,0));

    
     f.color = Color(1,0,0);
     f.h_align = TextShader::HorizAlign::HORIZ_ALIGN_LEFT;
     f.v_align = TextShader::VertAlign::VERT_ALIGN_TOP;
     ts.Draw3D(M3, V, P, "LEFT", f, true);
     
     f.color = Color(0,1,0);
     f.h_align = TextShader::HorizAlign::HORIZ_ALIGN_CENTER;
     f.v_align = TextShader::VertAlign::VERT_ALIGN_TOP;
     ts.Draw3D(M3, V, P, "CENTER", f, true);
     
     f.color = Color(0,0,1);
     f.h_align = TextShader::HorizAlign::HORIZ_ALIGN_RIGHT;
     f.v_align = TextShader::VertAlign::VERT_ALIGN_TOP;
     ts.Draw3D(M3, V, P, "RIGHT", f, true);
     
     
    M3 = Matrix4::Translation(Vector3(0,0,1));
    f.color = Color(1,1,1);
    f.h_align = TextShader::HorizAlign::HORIZ_ALIGN_CENTER;
    f.v_align = TextShader::VertAlign::VERT_ALIGN_BASELINE;
    f.size = 0.25;
    ts.Draw3D(M3, V, P, "Hello good buddy", f, true);
    
        
    //std::cout << mpos << " z = " << z_value_at_pixel(mpos) << std::endl;
}
