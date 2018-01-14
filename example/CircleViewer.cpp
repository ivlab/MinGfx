
#include "CircleViewer.h"

#include <nanogui/nanogui.h>
#include <iostream>


CircleViewer::CircleViewer() : GraphicsApp(1024,768, "Circle Simulation") {
    nanogui::FormHelper *gui = new nanogui::FormHelper(screen());
    nanogui::ref<nanogui::Window> window = gui->addWindow(Eigen::Vector2i(10, 10), "Simulation Controls");    
    pauseBtn_ = gui->addButton("Pause", std::bind(&CircleViewer::OnPauseBtnPressed, this));
    gui->addButton("Restart", std::bind(&CircleViewer::OnRestartBtnPressed, this));

    screen()->performLayout();

    simTime_ = 0.0;
    paused_ = false;
}


CircleViewer::~CircleViewer() {
    
}


void CircleViewer::UpdateSimulation(double dt) {
    if (!paused_) {
        simTime_ += dt;
        std::cout << "Update Simulation " << simTime_ << std::endl;
    }
}


void CircleViewer::OnRestartBtnPressed() {
    simTime_ = 0.0;
}

void CircleViewer::OnPauseBtnPressed() {
    paused_ = !paused_;
    if (paused_) {
        pauseBtn_->setCaption("Play");
    }
    else {
        pauseBtn_->setCaption("Pause");
    }
}


void CircleViewer::OnMouseMove(int x, int y) {
    std::cout << "Mouse moved to (" << x << ", " << y << ")" << std::endl; 
}

void CircleViewer::OnLeftMouseDown(int x, int y) {
    std::cout << "Left mouse button DOWN (" << x << ", " << y << ")" << std::endl; 
}

void CircleViewer::OnLeftMouseUp(int x, int y) {
    std::cout << "Left mouse button UP (" << x << ", " << y << ")" << std::endl; 
}

void CircleViewer::OnRightMouseDown(int x, int y) {
    std::cout << "Right mouse button DOWN (" << x << ", " << y << ")" << std::endl; 
}

void CircleViewer::OnRightMouseUp(int x, int y) {
    std::cout << "Right mouse button UP (" << x << ", " << y << ")" << std::endl; 
}

void CircleViewer::OnKeyDown(const char *c, int modifiers) {
    std::cout << "Key DOWN (" << c << ") modifiers=" << modifiers << std::endl; 
}

void CircleViewer::OnKeyUp(const char *c, int modifiers) {
    std::cout << "Key UP (" << c << ") modifiers=" << modifiers << std::endl; 
}

void CircleViewer::OnSpecialKeyDown(int key, int scancode, int modifiers) {
    std::cout << "Special Key DOWN key=" << key << " scancode=" << scancode << " modifiers=" << modifiers << std::endl; 
}

void CircleViewer::OnSpecialKeyUp(int key, int scancode, int modifiers) {
    std::cout << "Special Key UP key=" << key << " scancode=" << scancode << " modifiers=" << modifiers << std::endl; 
}


void CircleViewer::DrawUsingNanoVG(NVGcontext *ctx) {
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

void CircleViewer::DrawUsingOpenGL() {

}

