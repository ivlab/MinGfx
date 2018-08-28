/*
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 See corresponding header file for details.
 */

#include "gui_plus_opengl.h"

#include "default_shader.h"
#include "mesh.h"
#include "shader_program.h"
#include "texture2d.h"
#include "text_shader.h"
#include "unicam.h"
#include <stb_image.h>

#include <iostream>



class BasicMenu {
public:
    BasicMenu() : selected_(-1), button_pressed_(false) {}
    virtual ~BasicMenu() {}
    
    
    void Init(const std::string &title, const std::vector<std::string> &items) {
        // Graphics:
        ts_.Init(Platform::FindMinGfxDataFile("Futura_Medium_BT.ttf"), 42);
        
        
        // Content:
        title_ = title;
        items_ = items;
        
        
        // Style:
        title_format_.color = Color(1,1,1);
        title_format_.h_align = TextShader::HORIZ_ALIGN_LEFT;
        title_format_.v_align = TextShader::VERT_ALIGN_CENTER;
        title_bgcol_ = Color(85.0/255.0, 83.0/255.0, 83.0/255.0);
        title_highcol_ = Color(19.0/255.0, 0.0/255.0, 239.0/255.0);
        for (int i=0; i<title_.length(); i++) {
            title_[i] = std::toupper(title_[i]);
        }
        
        item_format_.color = Color(0,0,0);
        item_format_.h_align = TextShader::HORIZ_ALIGN_LEFT;
        item_format_.v_align = TextShader::VERT_ALIGN_CENTER;
        item_bgcol_ = Color(230.0/255.0, 230.0/255.0, 221.0/255.0);
        item_highcol_ = Color(255.0/255.0, 195.0/255.0, 0.0/255.0);

        presscol_ = Color(226.0/255.0, 0.0/255.0, 23.0/255.0);
        
        // Layout:
        item_sep_ = 0.008;
        padding_ = Vector2(0.05,0.05);
        depth_ = 0.03;
        z_epsilon_ = 0.001;
        
        Vector2 max_text_extents;
        for (int i=0; i<items_.size(); i++) {
            Vector2 text_extents = ts_.TextExtents(items_[i], item_format_);
            max_text_extents[0] = std::max(text_extents[0], max_text_extents[0]);
            max_text_extents[1] = std::max(text_extents[1], max_text_extents[1]);
        }

        // size of activatable box
        Vector3 menu_box_dims(max_text_extents[0] + 2.0*padding_[0],
                              max_text_extents[1] + 2.0*padding_[1],
                              depth_);
        
        float height = items_.size() * menu_box_dims[1] +
                       (items_.size() - 1) * item_sep_;
        
        // special case: title bar taller than items
        Vector2 title_extents = ts_.TextExtents(title_, title_format_);
        if (height < title_extents[0]) {
            height = title_extents[0] + 2.0*padding_[0];
        }
        

        // define AABoxes for menu items and the title bar
        // and transformation matrices to use for drawing text
        
        float y = 0.5*height - 0.5*menu_box_dims[1];
        for (int i=0; i<items_.size(); i++) {
        
            item_boxes_.push_back(AABB(Point3(0,y,0), menu_box_dims));
        
            Matrix4 M = Matrix4::Translation(Vector3(-0.5*menu_box_dims[0]+padding_[0], y, 0.5*depth_+z_epsilon_));
            item_text_xforms_.push_back(M);
            y -= menu_box_dims[1] + item_sep_;
        }
        
        title_box_ = AABB(Point3(-0.5*menu_box_dims[0] - 0.5*menu_box_dims[1], 0, 0),
                          Vector3(menu_box_dims[1], height, depth_));
        
        Vector3 t = Vector3(-0.5*menu_box_dims[0] - 0.5*menu_box_dims[1],
                            -0.5*height + padding_[0],
                            0.5*depth_ + z_epsilon_);
        title_text_xform_ = Matrix4::Translation(t) * Matrix4::RotationZ(M_PI_2);
        
        bgbox_ = AABB(Point3(-z_epsilon_, 0, -0.5*item_sep_+z_epsilon_),
                      Vector3(menu_box_dims[0]-z_epsilon_, height-2.0*z_epsilon_, menu_box_dims[2]-item_sep_));
    }
    
    
    void DrawBox(const Matrix4 &M, const Matrix4 &V, const Matrix4 &P, const AABB &box, const Color &col) {
        Matrix4 T = Matrix4::Translation((box.min() + 0.5*(box.max() - box.min())) - Point3::Origin());
        Matrix4 S = Matrix4::Scale(0.5*box.Dimensions());
        qs_.DrawCube(M * T * S, V, P, col);
    }
    
    
    void Draw(const Matrix4 &view_matix, const Matrix4 &proj_matrix) {
        DrawBox(xform_, view_matix, proj_matrix, bgbox_, item_bgcol_);
        
        for (int i=0; i<item_boxes_.size(); i++) {
            Color col = item_bgcol_;
            if (selected_ - 1 == i) {
                if (button_pressed_) {
                    col = presscol_;
                }
                else {
                    col = item_highcol_;
                }
            }
            DrawBox(xform_, view_matix, proj_matrix, item_boxes_[i], col);
        }
        Color col = title_bgcol_;
        if (selected_ == 0) {
            if (button_pressed_) {
                col = presscol_;
            }
            else {
                col = title_highcol_;
            }
        }
        DrawBox(xform_, view_matix, proj_matrix, title_box_, col);
        
        for (int i=0; i<items_.size(); i++) {
           ts_.Draw3D(xform_ * item_text_xforms_[i], view_matix, proj_matrix, items_[i], item_format_, true);
        }
        ts_.Draw3D(xform_ * title_text_xform_, view_matix, proj_matrix, title_, title_format_, true);
    }

    
    void OnPointerMove(const Ray &ray) {
        
        Ray ray_menuspace = xform_.Inverse() * ray;

        if ((button_pressed_) && (selected_ == 0)) {
            // In dragging state, move the menu
            Point3 p1 = drag_ray_.origin() + drag_itime_ * drag_ray_.direction();
            Point3 p2 = ray.origin() + drag_itime_ * ray.direction();
            xform_ = Matrix4::Translation(p2-p1) * xform_;
            drag_ray_ = ray;
        }
        else {
            // Update selection
            selected_ = -1;

            float best_itime = -1.0;
            float itime = -1.0;
            for (int i=0; i<item_boxes_.size(); i++) {
                if ((ray_menuspace.IntersectAABB(item_boxes_[i], &itime)) &&
                    (itime > 0.0) &&
                    ((best_itime == -1) || (itime < best_itime)))
                {
                    best_itime = itime;
                    selected_ = i+1;
                }
            }
            
            if ((ray_menuspace.IntersectAABB(title_box_, &itime)) &&
                (itime > 0.0) &&
                ((best_itime == -1) || (itime < best_itime)))
            {
                selected_ = 0;
                drag_ray_ = ray;
                drag_itime_ = itime;
            }
        }
    }
    
    void OnButtonDown() {
        button_pressed_ = true;
        if (selected_ > 0) {
            std::cout << "Menu selection for item " << selected_-1 << std::endl;
        }
    }
    
    void OnButtonUp() {
        button_pressed_ = false;
    }
    
    Matrix4 transform() { return xform_; }
    
    void set_transform(const Matrix4 &xform) { xform_ = xform; }
    
private:

    // Layout / content / style settings:
    
    Matrix4 xform_;
    float item_sep_;
    Vector2 padding_;
    float depth_;
    float z_epsilon_; // tiny offset for text to avoid z fighting
    
    std::string title_;
    TextShader::TextFormat title_format_;
    Color title_bgcol_;
    Color title_highcol_;

    std::vector<std::string> items_;
    TextShader::TextFormat item_format_;
    Color item_bgcol_;
    Color item_highcol_;
    
    Color presscol_;

    
    // UI implementation:
    
    AABB bgbox_;

    AABB title_box_;
    Matrix4 title_text_xform_;
    
    std::vector<AABB> item_boxes_;
    std::vector<Matrix4> item_text_xforms_;
    
    int selected_;
    bool button_pressed_;
    Ray drag_ray_;
    float drag_itime_;
    
    
    // Graphics implementation:
    
    TextShader ts_;
    QuickShapes qs_;
};




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


BasicMenu menu;


GuiPlusOpenGL::GuiPlusOpenGL() : GraphicsApp(1024,768, "Circle Simulation") {

    InitGraphicsContext();
    
    simTime_ = 0.0;
    paused_ = false;
    
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
    
    menu.OnPointerMove(pickRay);

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
    menu.OnButtonDown();

}

void GuiPlusOpenGL::OnRightMouseDrag(const Point2 &pos, const Vector2 &delta) {
    //std::cout << "Mouse dragged (right button) to " << pos << " delta from last frame " << delta << std::endl;
    Point2 mouseInNDC = PixelsToNormalizedDeviceCoords(pos);
    Point3 filmplanePt = GfxMath::ScreenToNearPlane(uniCam.view_matrix(), P, mouseInNDC);
    
    Matrix4 camMat = uniCam.view_matrix().Inverse();
    Point3 eye = camMat.ColumnToPoint3(3);
    
    pickRay = Ray(eye, filmplanePt - eye);
    menu.OnPointerMove(pickRay);

}

void GuiPlusOpenGL::OnRightMouseUp(const Point2 &pos) {
    //std::cout << "Right mouse button up at " << pos << std::endl;
    menu.OnButtonUp();

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
    std::string fname = Platform::FindMinGfxDataFile("Futura_Medium_BT.ttf");
    ts.Init(fname, 42);
        
    qs = new QuickShapes();

    P = Matrix4::Perspective(60.0, aspect_ratio(), 0.1, 10.0);
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
    
    
    std::vector<std::string> items;
    items.push_back("File");
    items.push_back("Edit");
    items.push_back("Layers");
    menu.Init("menu", items);
    menu.set_transform(Matrix4::Translation(Vector3(-1,1.1,0)));

    
}


void GuiPlusOpenGL::DrawUsingOpenGL() {
    
    V = uniCam.view_matrix();
    Matrix4 M = Matrix4::Translation(Vector3(-1,0,0)) * Matrix4::Scale(Vector3(0.5, 0.5, 0.5));

    // Draw several quick shapes
    float col[3] = {0.4, 0.4, 0.8};
    Matrix4 M2 = Matrix4::Translation(Vector3(1,1.5,0))*Matrix4::Scale(Vector3(0.2, 0.2, 0.2));
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
    
    
    qs->DrawArrow(Matrix4(), V, P, col, Point3(0,0,0), Vector3(-1,1.5,0), 0.01);
    
    qs->DrawLineSegment(Matrix4(), V, P, col, Point3(0,0,0), Point3(1, 1.5, 0), 0.01);
    
    qs->DrawAxes(Matrix4(), V, P);
    
    dShader.Draw(Matrix4(), V, P, &mesh1, mat1);
    dShader.Draw(Matrix4(), V, P, &mesh2, DefaultShader::MaterialProperties());
    
    
    float t;
    Point3 p;
    int id;
    if (pickRay.IntersectMesh(mesh1, &t, &p, &id)) {
        M2 = Matrix4::Translation(p - Point3::Origin()) * Matrix4::Scale(Vector3(0.025, 0.025, 0.025));
        qs->DrawSphere(M2, V, P, Color(1,0,1));
    }
    if (pickRay.IntersectMesh(mesh2, &t, &p, &id)) {
        M2 = Matrix4::Translation(p - Point3::Origin()) * Matrix4::Scale(Vector3(0.025, 0.025, 0.025));
        qs->DrawSphere(M2, V, P, Color(1,1,0));
    }
    
    uniCam.Draw(P);
    
    
    Matrix4 M3 = Matrix4::Translation(Vector3(-1,0,0));
    TextShader::TextFormat f;
    
    
    f.color = Color(1,1,0);
    f.h_align = TextShader::HORIZ_ALIGN_CENTER;
    f.v_align = TextShader::VERT_ALIGN_TOP;
    ts.Draw3D(M3, V, P, "TOPgg", f, true);
    
    f.color = Color(0,1,1);
    f.h_align = TextShader::HORIZ_ALIGN_CENTER;
    f.v_align = TextShader::VERT_ALIGN_CENTER;
    ts.Draw3D(M3, V, P, "V_CENTERyy", f, true);
    
    f.color = Color(1,0,1);
    f.h_align = TextShader::HORIZ_ALIGN_CENTER;
    f.v_align = TextShader::VERT_ALIGN_BOTTOM;
    ts.Draw3D(M3, V, P, "BOTTOMgg", f, true);
    
    f.color = Color(1,1,1);
    f.h_align = TextShader::HORIZ_ALIGN_CENTER;
    f.v_align = TextShader::VERT_ALIGN_BASELINE;
    ts.Draw3D(M3, V, P, "Hello good buddy", f, true);
    
    qs->DrawLineSegment(M3, V, P, col, Point3(0,0,0), Point3(1, 0, 0), 0.01);

    M3 = M3 * Matrix4::Translation(Vector3(0,0.5,0));

    
     f.color = Color(1,0,0);
     f.h_align = TextShader::HORIZ_ALIGN_LEFT;
     f.v_align = TextShader::VERT_ALIGN_TOP;
     ts.Draw3D(M3, V, P, "LEFT", f, true);
     
     f.color = Color(0,1,0);
     f.h_align = TextShader::HORIZ_ALIGN_CENTER;
     f.v_align = TextShader::VERT_ALIGN_TOP;
     ts.Draw3D(M3, V, P, "CENTER", f, true);
     
     f.color = Color(0,0,1);
     f.h_align = TextShader::HORIZ_ALIGN_RIGHT;
     f.v_align = TextShader::VERT_ALIGN_TOP;
     ts.Draw3D(M3, V, P, "RIGHT", f, true);
     
     
    M3 = Matrix4::Translation(Vector3(0,0,1));
    f.color = Color(1,1,1);
    f.h_align = TextShader::HORIZ_ALIGN_CENTER;
    f.v_align = TextShader::VERT_ALIGN_BASELINE;
    f.size = 0.25;
    ts.Draw3D(M3, V, P, "Hello good buddy", f, true);
    
    
    menu.Draw(V, P);
    
        
    //std::cout << mpos << " z = " << z_value_at_pixel(mpos) << std::endl;
}
